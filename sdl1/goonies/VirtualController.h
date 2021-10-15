#ifndef __VIRTUAL_CONTROLLER
#define __VIRTUAL_CONTROLLER

#define VC_UP  0
#define VC_RIGHT 1
#define VC_DOWN  2
#define VC_LEFT  3

class VirtualController
{
    public:
        bool m_joystick[4];
        bool m_button[2];
        bool m_pause;
        bool m_quit;

        bool m_old_joystick[4];
        bool m_old_button[2];
        bool m_old_pause;
        bool m_old_quit;

        void reset(void)
        {
            m_old_joystick[0] = m_joystick[0] = false;
            m_old_joystick[1] = m_joystick[1] = false;
            m_old_joystick[2] = m_joystick[2] = false;
            m_old_joystick[3] = m_joystick[3] = false;

            m_old_button[0] = m_button[0] = false;
            m_old_button[1] = m_button[1] = false;

            m_old_pause = m_pause = false;
            m_old_quit = m_quit = false;
        } 
		
        void new_cycle(void)
        {
            m_old_joystick[0] = m_joystick[0];
            m_old_joystick[1] = m_joystick[1];
            m_old_joystick[2] = m_joystick[2];
            m_old_joystick[3] = m_joystick[3];

            m_old_button[0] = m_button[0];
            m_old_button[1] = m_button[1];

            m_old_pause = m_pause;
            m_old_quit = m_quit;
        }
};

#endif

