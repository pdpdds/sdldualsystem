/**
 * @file  camoto/iff.hpp
 * @brief RIFF/IFF/RIFX reader/writer.
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

#ifndef _CAMOTO_IFF_HPP_
#define _CAMOTO_IFF_HPP_

#include <vector>
#include <camoto/stream.hpp>

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

namespace camoto {

class DLL_EXPORT IFF
{
	public:
		typedef std::string fourcc;

		enum Filetype {
			/// Standard Microsoft RIFF file with length values in little-endian
			Filetype_RIFF = 0x00,

			/// Standard EA/Amiga IFF file with length values in big-endian
			Filetype_IFF  = 0x01,

			/// Same as RIFF but with no padding bytes between chunks (non-standard)
			Filetype_RIFF_Unpadded = 0x02,

			/// Same as IFF but with no padding bytes between chunks (non-standard)
			Filetype_IFF_Unpadded = 0x03,
		};
};

class DLL_EXPORT IFFReader: public IFF
{
	public:
		IFFReader(stream::input_sptr iff, Filetype filetype);

		/// Return to the file root.
		/**
		 * From this point, the next step is always to open() the RIFF chunk.
		 */
		void root();

		/// Get all the chunk names that can be opened or selected at this point.
		/**
		 * Any of the returned values can be immediately passed to seek() or open().
		 */
		std::vector<fourcc> list();

		/// Seek to the start of the given chunk.
		/**
		 * @param name
		 *   fourcc of the chunk to select.
		 *
		 * @return The length of the selected chunk, in bytes.
		 *
		 * @post The stream passed to the constructor has its read pointer set
		 *   to the start of the chunk's content.
		 */
		stream::len seek(const fourcc& name);

		/// Seek to the start of the given chunk.
		/**
		 * @param index
		 *   Index into the chunk list returned by list().
		 *
		 * @return The length of the selected chunk, in bytes.
		 *
		 * @post The stream passed to the constructor has its read pointer set
		 *   to the start of the chunk's content.
		 */
		stream::len seek(unsigned int index);

		/// Open a subchunk within the current one.
		/**
		 * @param name
		 *   fourcc of the chunk to select.
		 *
		 * @param type
		 *   If supplied, the first four bytes in the opened chunk are read and
		 *   placed here, and subchunks are listed from this point on.  This is
		 *   the case for RIFF and LIST chunks.  If this parameter is NULL, then
		 *   this special fourcc is not read and the subchunks (if any) start
		 *   immediately.
		 *
		 * @return The length of the newly opened chunk.
		 *
		 * @post The stream passed to the constructor has its read pointer set
		 *   to the start of the chunk's content.
		 */
		stream::len open(const fourcc& name, fourcc *type);

		/// Open a subchunk within the current one, by index.
		/**
		 * @param index
		 *   Index into the chunk list returned by list().
		 *
		 * @param type
		 *   If supplied, the first four bytes in the opened chunk are read and
		 *   placed here, and subchunks are listed from this point on.  This is
		 *   the case for RIFF and LIST chunks.  If this parameter is NULL, then
		 *   this special fourcc is not read and the subchunks (if any) start
		 *   immediately.
		 *
		 * @return The length of the newly opened chunk.
		 *
		 * @post The stream passed to the constructor has its read pointer set
		 *   to the start of the chunk's content.
		 */
		stream::len open(unsigned int index, fourcc *type);

	protected:
		stream::input_sptr iff;         ///< File to read
		Filetype filetype;              ///< Type of file (RIFF, IFF, etc.)
		struct Chunk {
			fourcc name;
			stream::pos start;
			stream::len len;
		};
		std::vector<Chunk> chunks;

		void loadChunks(stream::len lenChunk);
};

class DLL_EXPORT IFFWriter: public IFF
{
	public:
		IFFWriter(stream::output_sptr iff, Filetype filetype);

		/// Open a subchunk within the current one.
		/**
		 * @param name
		 *   fourcc of the chunk to create.
		 *
		 * @post The stream passed to the constructor has its read pointer set
		 *   to the start of the chunk's content.
		 */
		void begin(const fourcc& name);

		/// Open a subchunk within the current one.
		/**
		 * @param name
		 *   fourcc of the chunk to create.
		 *
		 * @param type
		 *   File type for RIFF or LIST chunks.  Causes this fourcc to be written
		 *   as the first four bytes in the chunk, with other subchunks following.
		 */
		void begin(const fourcc& name, const fourcc& type);

		/// Finish the current chunk.
		/**
		 * From this point on a sibling chunk can be started with a call to begin()
		 * or the parent chunk can be finalised with another call to end().
		 *
		 * @post The length of the given chunk is updated, but the write pointer
		 *   is returned to its original location.
		 */
		void end();

	protected:
		stream::output_sptr iff;        ///< File to write
		Filetype filetype;              ///< Type of file (RIFF, IFF, etc.)
		std::vector<stream::pos> chunk; ///< Offset of start of most recent chunk
};

} // namespace camoto

#endif // _CAMOTO_IFF_HPP_
