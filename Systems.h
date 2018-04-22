#ifndef SYSTEMS_H_
#define SYSTEMS_H_ 

#include "util.h"
#include "SystemSettings.h"

namespace Systems {
    class System {
        public:
            System();
            System(const uint8 id);

            /* Setters (to be implemented by each extending system class) */
            uint8 SetDir(int8 direction);
            uint8 SetDuty(int8 direction);
            uint8 SetSetPoint(uint8 index, float setpoint);

            /* Getters */
            uint8 GetID() { return ID; }
            uint8 GetState() { return state; }
            float GetDuty() { return duty; }
            float* GetSetPoints() { return setpoints; }
            float* GetFeedBacks() { return feedbacks; }

        private:
            uint8 ID;
            int8 state;
            float duty;
            float setpoints[SETPOINT_NUM];
            float feedbacks[FEEDBACK_NUM];
    };

    static uint8 InitializeSystems();
    static System* SystemList;
};


#endif /* SYSTEMS_H_ */
