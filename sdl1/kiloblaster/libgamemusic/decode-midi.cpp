/**
 * @file  decode-midi.cpp
 * @brief Format decoder for Standard MIDI Format (SMF) MIDI data.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <deque>
#include <camoto/stream.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp>
#include <camoto/gamemusic/patch-midi.hpp>
#include <camoto/gamemusic/patch-opl.hpp>
#include "patch-adlib.hpp"
#include "decode-midi.hpp"
#include "track-split.hpp"

using namespace camoto;
using namespace camoto::gamemusic;

/// Frequency to use for percussion notes.
const unsigned int PERC_FREQ = 440000;

/// Default to a grand piano until a patch change event arrives.
const unsigned int MIDI_DEFAULT_PATCH = 0;

/// Convert a MIDI pitchbend value into a number of semitones, possibly negative
/**
 * @param bend
 *   MIDI bend amount.  8192 is middle, 0 is -2 semitones, 16384 is +2 semitones
 */
inline double midiBendToSemitone(unsigned int bend)
{
	return (bend - 8192.0) / 4096.0;
}

/// MusicReader class that understands MIDI data.
class MIDIDecoder
{
	public:
		/// Constructor
		/**
		 * @param input
		 *   Data stream containing the MIDI data.
		 *
		 * @param midiFlags
		 *   One or more flags.  Use MIDIFlags::Default unless the MIDI
		 *   data is unusual in some way.
		 *
		 * @param initialTempo
		 *   Initial tempo of the song.  The number of ticks in a quarter note is
		 *   particularly important for a MIDI file to get the beat/bar arrangement
		 *   correct.
		 */
		MIDIDecoder(stream::input_sptr input, unsigned int midiFlags,
			const Tempo& initialTempo);

		virtual ~MIDIDecoder();

		/// Read the input data and set events and instruments accordingly.
		/**
		 * This function will call the callback's readNextPair() function repeatedly
		 * until it returns false.  It will populate the events and instruments in
		 * the supplied Music instance.
		 *
		 * @throw stream:error
		 *   If the input data could not be read or converted for some reason.
		 */
		MusicPtr decode();

	protected:
		stream::input_sptr input;         ///< MIDI data to read
		unsigned long lastDelay[MIDI_CHANNEL_COUNT]; ///< Ticks since last event on this channel
		unsigned long totalDelay;         ///< Running total of all delays
		std::vector<bool> noteOnTrack;    ///< true if note playing on given track
		uint8_t lastEvent;                ///< Last event (for MIDI running status)
		std::deque<EventPtr> eventBuffer; ///< List of events to return in readNextEvent()
		PatchBankPtr patches;             ///< Cached patches populated by getPatchBank()
		unsigned int midiFlags;           ///< Flags supplied in constructor
		Tempo curTempo;                   ///< Last set song tempo

		/// For each of the percussion notes, which instrument are we using?
		uint8_t percMap[MIDI_NOTES];

		/// Current instrument (not MIDI patch) on each MIDI channel
		uint8_t currentInstrument[MIDI_CHANNEL_COUNT];

		/// Current pitchbend level on each MIDI channel (-8192 to 8191)
		int16_t currentPitchbend[MIDI_CHANNEL_COUNT];

		/// Is this note being played on this channel?
		bool activeNotes[MIDI_CHANNEL_COUNT][MIDI_NOTES];

		/// Read a variable-length integer from MIDI data.
		/**
		 * Reads up to four bytes from the MIDI stream, returning a 28-bit
		 * integer.
		 */
		uint32_t readMIDINumber();

		/// Set the MIDI patch in use on the given channel.
		/**
		 * This will add a new entry into the patchbank if the given patch does
		 * not exist.  Otherwise it will reuse an existing entry in the patchbank.
		 *
		 * @param patches
		 *   PatchBank to search within and possibly append to.
		 *
		 * @param midiChannel
		 *   MIDI channel (0-15).
		 *
		 * @param midiPatch
		 *   MIDI instrument number (0-127).
		 */
		void setInstrument(PatchBankPtr& patches, unsigned int midiChannel,
			unsigned int midiPatch);
};

MusicPtr camoto::gamemusic::midiDecode(stream::input_sptr input,
	unsigned int flags, const Tempo& initialTempo)
{
	MIDIDecoder decoder(input, flags, initialTempo);
	return decoder.decode();
}


MIDIDecoder::MIDIDecoder(stream::input_sptr input, unsigned int midiFlags,
	const Tempo& initialTempo)
	:	input(input),
		totalDelay(0),
		lastEvent(0),
		midiFlags(midiFlags),
		curTempo(initialTempo)
{
	memset(this->percMap, 0xFF, sizeof(this->percMap));
	memset(this->currentInstrument, 0, sizeof(this->currentInstrument));
	memset(this->activeNotes, 0, sizeof(this->activeNotes));
	for (unsigned int i = 0; i < MIDI_CHANNEL_COUNT; i++) {
		this->currentPitchbend[i] = 8192;
	}
}

MIDIDecoder::~MIDIDecoder()
{
}

MusicPtr MIDIDecoder::decode()
{
	MusicPtr music(new Music());
	music->patches.reset(new PatchBank());
	music->initialTempo = this->curTempo;

	PatternPtr pattern(new Pattern());
	music->patterns.push_back(pattern);
	music->patternOrder.push_back(0);

	for (unsigned int c = 0; c < MIDI_CHANNEL_COUNT; c++) {
		TrackInfo t;
		t.channelType = TrackInfo::MIDIChannel;
		t.channelIndex = c;
		music->trackInfo.push_back(t);

		TrackPtr track(new Track());
		pattern->push_back(track);
		lastDelay[c] = 0;
	}

	std::vector<NoteOnEvent *> eventsOPL7, eventsOPL8;
	try {
		bool eof = false;
		do {
			uint32_t delay = this->readMIDINumber();
			for (unsigned int c = 0; c < MIDI_CHANNEL_COUNT; c++) {
				lastDelay[c] += delay;
			}
			this->totalDelay += delay;

			if (this->midiFlags & MIDIFlags::CMFExtensions) {
				if (delay) {
					// There is a delay, so update any concurrent OPL events.  Whichever
					// one is played last has its pitch copied onto the others, as this
					// is the way it ends up on the OPL chip with SBFMDRV.
					// We do lose the original note pitch by doing this, but then this is
					// how the song actually sounds...
					unsigned int pitch = 0;
					for (std::vector<NoteOnEvent *>::reverse_iterator
						i = eventsOPL7.rbegin(); i != eventsOPL7.rend(); i++
					) {
						NoteOnEvent*& nev = *i;
						if (!pitch) pitch = nev->milliHertz;
						else nev->milliHertz = pitch;
					}
					pitch = 0;
					for (std::vector<NoteOnEvent *>::reverse_iterator
						i = eventsOPL8.rbegin(); i != eventsOPL8.rend(); i++
					) {
						NoteOnEvent*& nev = *i;
						if (!pitch) pitch = nev->milliHertz;
						else nev->milliHertz = pitch;
					}
					eventsOPL7.clear();
					eventsOPL8.clear();
				}
			}

			uint8_t event, evdata;
			this->input >> u8(event);
			if (event & 0x80) {
				// If the high bit is set it's a normal event
				if ((event & 0xF0) != 0xF0) {
					// 0xF0 events do not change the running status
					this->lastEvent = event;
				}
				this->input >> u8(evdata);
			} else {
				// The high bit is unset, so this is actually the first data
				// byte for a new event, of the same type as the last event.
				evdata = event;
				event = this->lastEvent;
			}

			// Handle short aftertouch events if the flag was given
			if (this->midiFlags & MIDIFlags::ShortAftertouch) {
				// Change key aftertouch to channel aftertouch
				if ((event & 0xF0) == 0xA0) event = 0xD0 | (event & 0x0F);
			}

			bool deepTremolo = (this->midiFlags & MIDIFlags::CMFExtensions) ? true : false;
			bool deepVibrato = (this->midiFlags & MIDIFlags::CMFExtensions) ? true : false;
			unsigned int midiChannel = event & 0x0F;
			unsigned int& track = midiChannel;
			switch (event & 0xF0) {
				case 0x80: { // Note off (two data bytes)
					if (evdata >= MIDI_NOTES) {
						throw stream::error(createString("MIDI note " << (int)evdata
							<< " out of range (max " << MIDI_NOTES << ")"));
					}

					const uint8_t& note = evdata;
					uint8_t velocity;
					this->input >> u8(velocity);

					TrackEvent te;
					te.delay = this->lastDelay[track];
					this->lastDelay[track] = 0;

					if (
						(this->midiFlags & MIDIFlags::CMFExtensions)
						&& (midiChannel >= 12)
					) {
						// OPL percussive instruments may have had their pitched changed
						// (if two notes sharing an OPL channel were played at the same
						// moment), so instead of tracking that, just turn all the notes
						// on the channel off.  Technically this might turn other notes
						// off too soon, but the OPL can only play one note at a time on
						// these channels anyway.
						NoteOffEvent *ev = new NoteOffEvent();
						te.event.reset(ev);
					} else {
						SpecificNoteOffEvent *ev = new SpecificNoteOffEvent();
						te.event.reset(ev);
						ev->milliHertz = midiToFreq(note);
					}
					pattern->at(track)->push_back(te);

					// Record this note as inactive on the channel
					this->activeNotes[midiChannel][note] = false;

					break;
				}
				case 0x90: { // Note on (two data bytes)
					if (evdata >= MIDI_NOTES) {
						throw stream::error(createString("MIDI note " << (int)evdata
							<< " out of range (max " << MIDI_NOTES << ")"));
					}

					uint8_t& note = evdata;
					uint8_t velocity;
					this->input >> u8(velocity);

					if ((velocity == 0) || (this->activeNotes[midiChannel][note])) { // note off
						// Only generate a note-off event if the note was actually on, or
						// if it was on and is being played again without a noteoff
						TrackEvent te;
						te.delay = this->lastDelay[track];
						this->lastDelay[track] = 0;

						if (
							(this->midiFlags & MIDIFlags::CMFExtensions)
							&& (midiChannel >= 12)
						) {
							// OPL percussive instruments may have had their pitched changed
							// (if two notes sharing an OPL channel were played at the same
							// moment), so instead of tracking that, just turn all the notes
							// on the channel off.  Technically this might turn other notes
							// off too soon, but the OPL can only play one note at a time on
							// these channels anyway.
							NoteOffEvent *ev = new NoteOffEvent();
							te.event.reset(ev);
						} else {
							SpecificNoteOffEvent *ev = new SpecificNoteOffEvent();
							te.event.reset(ev);
							ev->milliHertz = midiToFreq(note);
						}
						pattern->at(track)->push_back(te);

						// Record this note as inactive on the channel
						this->activeNotes[midiChannel][note] = false;
					}

					if (velocity != 0) {
						TrackEvent te;
						te.delay = this->lastDelay[track];
						this->lastDelay[track] = 0;
						NoteOnEvent *ev = new NoteOnEvent();
						te.event.reset(ev);
						// MIDI is 1-127, we are 1-255 (MIDI velocity 0 is note off)
						ev->velocity = (velocity << 1) | (velocity >> 6);
						if (((this->midiFlags & MIDIFlags::Channel10NoPerc) == 0) && (midiChannel == 9)) {
							if (this->percMap[note] == 0xFF) {
								// Need to allocate a new instrument for this percussion note
								MIDIPatchPtr newPatch(new MIDIPatch());
								newPatch->percussion = true;
								newPatch->midiPatch = note;
								this->percMap[note] = music->patches->size();
								music->patches->push_back(newPatch);
							}
							ev->milliHertz = PERC_FREQ;
							ev->instrument = this->percMap[note];
						} else {
							ev->milliHertz = midiToFreq(note + midiBendToSemitone(this->currentPitchbend[midiChannel]));
							ev->instrument = this->currentInstrument[midiChannel];
						}
						if (ev->instrument >= music->patches->size()) {
							// A note is sounding without a patch change event ever arriving,
							// so use a default instrument
							this->setInstrument(music->patches, midiChannel, MIDI_DEFAULT_PATCH);
						}

						if (this->midiFlags & MIDIFlags::CMFExtensions) {
							// For CMF files, the order of events on these channels is
							// important as each one affects the other.
							if ((midiChannel == 12) || (midiChannel == 15)) {
								eventsOPL7.push_back(ev);
							} else if ((midiChannel == 13) || (midiChannel == 14)) {
								eventsOPL8.push_back(ev);
							}
						}
						pattern->at(track)->push_back(te);

						// Record this note as active on the channel
						this->activeNotes[midiChannel][note] = true;
					}
					break;
				}
				case 0xA0: { // Polyphonic key pressure (two data bytes)
					const uint8_t& note = evdata;
					uint8_t pressure;
					this->input >> u8(pressure);

					TrackEvent te;
					te.delay = this->lastDelay[track];
					this->lastDelay[track] = 0;

					SpecificNoteEffectEvent *ev = new SpecificNoteEffectEvent();
					te.event.reset(ev);
					ev->type = EffectEvent::Volume;
					ev->data = (pressure << 1) | (pressure >> 6);
					ev->milliHertz = midiToFreq(note);
					pattern->at(track)->push_back(te);
					break;
				}
				case 0xB0: { // Controller (two data bytes)
					uint8_t value;
					// controller index is in evdata
					this->input >> u8(value);
					switch (evdata) {
						case 0x63: {
							bool newVibrato = value & 1;
							bool newTremolo = value & 2;
							if (newVibrato != deepVibrato) {
								TrackEvent te;
								te.delay = this->lastDelay[track];
								this->lastDelay[track] = 0;
								ConfigurationEvent *ev = new ConfigurationEvent();
								te.event.reset(ev);
								ev->configType = ConfigurationEvent::EnableDeepVibrato;
								ev->value = newVibrato ? 1 : 0;
								pattern->at(track)->push_back(te);
								deepVibrato = newVibrato;
							}
							if (newTremolo != deepTremolo) {
								TrackEvent te;
								te.delay = this->lastDelay[track];
								this->lastDelay[track] = 0;
								ConfigurationEvent *ev = new ConfigurationEvent();
								te.event.reset(ev);
								ev->configType = ConfigurationEvent::EnableDeepTremolo;
								ev->value = newTremolo ? 1 : 0;
								pattern->at(track)->push_back(te);
								deepTremolo = newTremolo;
							}
							break;
						}
						case 0x67: {
							TrackEvent te;
							te.delay = this->lastDelay[track];
							this->lastDelay[track] = 0;
							ConfigurationEvent *ev = new ConfigurationEvent();
							te.event.reset(ev);
							ev->configType = ConfigurationEvent::EnableRhythm;
							ev->value = value;
							pattern->at(track)->push_back(te);
							break;
						}
						case 0x68: {
							TrackEvent te;
							te.delay = lastDelay[track];
							lastDelay[track] = 0;
							PolyphonicEffectEvent *ev = new PolyphonicEffectEvent();
							te.event.reset(ev);
							ev->type = (EffectEvent::EffectType)PolyphonicEffectEvent::PitchbendChannel;
							ev->data = midiSemitonesToPitchbend(value / 128.0);
							pattern->at(track)->push_back(te);
							break;
						}
						case 0x69: {
							TrackEvent te;
							te.delay = lastDelay[track];
							lastDelay[track] = 0;
							PolyphonicEffectEvent *ev = new PolyphonicEffectEvent();
							te.event.reset(ev);
							ev->type = (EffectEvent::EffectType)PolyphonicEffectEvent::PitchbendChannel;
							ev->data = midiSemitonesToPitchbend(-value / 128.0);
							pattern->at(track)->push_back(te);
							break;
						}
						default:
							std::cout << "Ignoring unknown MIDI controller 0x" << std::hex
								<< (int)evdata << std::dec << std::endl;
							break;
					}
					break;
				}
				case 0xC0: { // Instrument change (one data byte)
					this->setInstrument(music->patches, midiChannel, evdata);
					break;
				}
				case 0xD0: { // Channel pressure (one data byte)
					TrackEvent te;
					te.delay = this->lastDelay[track];
					this->lastDelay[track] = 0;
					PolyphonicEffectEvent *ev = new PolyphonicEffectEvent();
					te.event.reset(ev);
					ev->type = (EffectEvent::EffectType)PolyphonicEffectEvent::VolumeChannel;
					// MIDI is 1-127, we are 1-255 (MIDI velocity 0 is note off)
					ev->data = (evdata << 1) | (evdata >> 6);
					pattern->at(track)->push_back(te);

					break;
				}
				case 0xE0: { // Pitch bend (two data bytes)
					uint8_t msb;
					this->input >> u8(msb);
					// Only lower seven bits are used in each byte
					unsigned int bend = ((msb & 0x7F) << 7) | (evdata & 0x7F);

					TrackEvent te;
					te.delay = this->lastDelay[track];
					this->lastDelay[track] = 0;
					PolyphonicEffectEvent *ev = new PolyphonicEffectEvent();
					te.event.reset(ev);
					ev->type = (EffectEvent::EffectType)PolyphonicEffectEvent::PitchbendChannel;
					ev->data = bend;
					pattern->at(track)->push_back(te);
					break;
				}
				case 0xF0: // System message (arbitrary data bytes)
					switch (event) {
						case 0xF0: { // Sysex
							while ((evdata & 0x80) == 0) this->input >> u8(evdata);
							// This will have read in the terminating EOX (0xF7) message too
							break;
						}
						case 0xF1: // MIDI Time Code Quarter Frame
							this->input->seekg(1, stream::cur); // message data (ignored)
							break;
						case 0xF2: // Song position pointer
							this->input->seekg(2, stream::cur); // message data (ignored)
							break;
						case 0xF3: // Song select
							this->input->seekg(1, stream::cur); // message data (ignored)
							std::cout << "Warning: MIDI Song Select is not implemented." << std::endl;
							break;
						case 0xF6: // Tune request
							break;
						case 0xF7: // End of System Exclusive (EOX) - should never be read, should be absorbed by Sysex handling code
							break;

							// These messages are "real time", meaning they can be sent
							// between the bytes of other messages - but we're lazy and don't
							// handle these here (hopefully they're not necessary in a MIDI
							// file, and even less likely to occur in a CMF.)
						case 0xF8: // Timing clock (sent 24 times per quarter note, only when playing)
						case 0xFA: // Start
						case 0xFB: // Continue
						case 0xFE: // Active sensing (sent every 300ms or MIDI connection assumed lost)
							break;
						case 0xFC: // Stop
							eof = true;
							break;
						case 0xFF: { // System reset, used as meta-events in a MIDI file
							uint32_t len = this->readMIDINumber();
							switch (evdata) {
								case 0x2F: // end of track
									eof = true;
									break;
								case 0x51: { // set tempo
									if (len != 3) {
										std::cerr << "Set tempo event had invalid length!" << std::endl;
										this->input->seekg(len, stream::cur); // message data (ignored)
										break;
									}
									unsigned long usPerQuarterNote = 0;
									uint8_t n;
									this->input >> u8(n); usPerQuarterNote  = n << 16;
									this->input >> u8(n); usPerQuarterNote |= n << 8;
									this->input >> u8(n); usPerQuarterNote |= n;

									if (this->totalDelay == 0) {
										// No events yet, update initial tempo
										music->initialTempo.usPerQuarterNote(usPerQuarterNote);
									} else {
										const unsigned int track = 0;
										TrackEvent te;
										te.delay = this->lastDelay[track];
										this->lastDelay[track] = 0;
										TempoEvent *ev = new TempoEvent();
										te.event.reset(ev);
										ev->tempo.usPerQuarterNote(usPerQuarterNote);
										pattern->at(track)->push_back(te);
									}
									break;
								}
								case 0x7F: { // AdLib MDI
									if (len < 3+2+1) break; // too short for opcode + data
									uint8_t mfgId1;
									uint16_t mfgId2;
									this->input
										>> u8(mfgId1)
										>> u16be(mfgId2)
									;
									len -= 3;
									if ((mfgId1 == 0) && (mfgId2 == 0x3F)) {
										// AdLib MDI opcode
										uint16_t opcode;
										this->input >> u16be(opcode);
										len -= 2;
										switch (opcode) {
											case 1: { // Instrument change
												uint8_t channel;
												this->input >> u8(channel);
												len--;
												if (len < 28) {
													std::cout << "[decode-midi] Got AdLib patch change "
														"but register data is only " << len
														<< " bytes long (expected 28)" << std::endl;
													break;
												}
												if (channel > 15) {
													std::cout << "[decode-midi] Got AdLib patch change "
														"for out-of-range channel #" << (int)channel
														<< ", ignoring." << std::endl;
													break;
												}
												OPLPatchPtr oplPatch(new OPLPatch);
												readAdLibPatch<uint8_t>(this->input, &*oplPatch);
												len -= 28;
												this->currentInstrument[channel] = music->patches->size();
												music->patches->push_back(oplPatch);

												// Switch this track from MIDI instruments to OPL ones
												TrackInfo& t = music->trackInfo[channel];
												t.channelType = TrackInfo::OPLChannel;
												break;
											}
											case 2: { // Rhythm-mode change
												uint8_t soundMode;
												this->input >> u8(soundMode);
												len--;
												TrackEvent te;
												te.delay = this->lastDelay[track];
												this->lastDelay[track] = 0;
												ConfigurationEvent *ev = new ConfigurationEvent();
												te.event.reset(ev);
												ev->configType = ConfigurationEvent::EnableRhythm;
												ev->value = (soundMode == 0) ? 0 : 1;
												pattern->at(track)->push_back(te);
												break;
											}
											case 3: { // Pitchbend range change
												uint8_t pitchBRange;
												this->input >> u8(pitchBRange);
												len--;
												std::cout << "[decode-midi] AdLib pitchbend range "
													"change (to " << (int)pitchBRange
													<< ") unimplemented!" << std::endl;
												break;
											}
										}
									} else {
										// Unknown manufacturer ID, skip
										std::cout << "[decode-midi] Unknown manufacturer ID in "
											"MIDI meta-event 0x7F: " << std::hex << (int)mfgId1
											<< "/" << mfgId2 << std::dec << std::endl;
									}
									this->input->seekg(len, stream::cur); // message data (ignored)
									break;
								}
								default:
									std::cout << "[decode-midi] Unknown MIDI meta-event 0x"
										<< std::hex << (int)evdata << std::dec << std::endl;
									this->input->seekg(len, stream::cur); // message data (ignored)
									break;
							}
							break;
						}
						default:
							std::cout << "[decode-midi] Unknown MIDI system command 0x"
								<< std::hex << (int)event << std::dec << std::endl;
							break;
					}
					break;
				default:
					std::cout << "[decode-midi] Unknown MIDI command 0x" << std::hex
						<< (int)event << std::dec << std::endl;
					break;
			}
		} while (!eof);
	} catch (const stream::incomplete_read&) {
		// reached eof
	}

	// Add dummy events for any final delays
	unsigned int trackCount = MIDI_CHANNEL_COUNT;
	for (int track = trackCount - 1; track >= 0; track--) {
		if (lastDelay[track] == totalDelay) {
			// This track is unused
			music->trackInfo.erase(music->trackInfo.begin() + track);
			music->patterns.at(0)->erase(music->patterns.at(0)->begin() + track);
		} else if (lastDelay[track]) {
			// This track has a trailing delay
			TrackEvent te;
			te.delay = lastDelay[track];
			lastDelay[track] = 0;
			ConfigurationEvent *ev = new ConfigurationEvent();
			te.event.reset(ev);
			ev->configType = ConfigurationEvent::EmptyEvent;
			ev->value = 0;
			pattern->at(track)->push_back(te);
		}
	}

	music->ticksPerTrack = this->totalDelay;

	splitPolyphonicTracks(music);
	return music;
}

uint32_t MIDIDecoder::readMIDINumber()
{
	// Make sure this->setMIDIStream() has been called
	assert(this->input);

	uint32_t val = 0;

	if (this->midiFlags & MIDIFlags::AdLibMUS) {
		for (int i = 0; i < 255; i++) { // 255 is just an arbitrary limit
			uint8_t n;
			this->input >> u8(n);
			if (n == 0xF8) {
				val += 240;
			} else {
				val += n;
				break;
			}
		}
	} else {
		for (int i = 0; i < 4; i++) {
			uint8_t n;
			this->input >> u8(n);
			val <<= 7;
			val |= (n & 0x7F); // ignore the MSB
			if (!(n & 0x80)) break; // last byte has the MSB unset
		}
	}
	return val;
}

void MIDIDecoder::setInstrument(PatchBankPtr& patches, unsigned int midiChannel,
	unsigned int midiPatch)
{
	bool found = false;
	int n = 0;
	for (PatchBank::const_iterator i = patches->begin(); i != patches->end(); i++, n++) {
		MIDIPatchPtr p = boost::dynamic_pointer_cast<MIDIPatch>(*i);
		if (!p) continue;
		if (p->percussion) continue;
		if (p->midiPatch == midiPatch) {
			this->currentInstrument[midiChannel] = n;
			found = true;
			break;
		}
	}
	if (!found) {
		// Have to allocate a new instrument
		MIDIPatchPtr newPatch(new MIDIPatch());
		newPatch->percussion = false;
		newPatch->midiPatch = midiPatch;
		this->currentInstrument[midiChannel] = patches->size();
		patches->push_back(newPatch);
	}
	return;
}
