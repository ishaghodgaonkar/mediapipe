// Here are tools to modify the coordinates incoming frames.
// These calculators are useful when you don't need to do calculation on every single frame
// For example, the rolling average calculator can be combined with the coordinate limitation calculator to take the average of every 10 frames

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

#include <time.h>  


const int SEND_EVERY_X_FRAMES = 3;
const int SEND_EVERY_X_SECONDS = 9;

namespace mediapipe{

    namespace{
        constexpr char NormalizedLandmarks[] = "LANDMARKS";
    }


    class LimitationCalculator : public CalculatorBase {
        public:
        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            cc->Outputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Process(CalculatorContext* cc){
            if(! cc -> Inputs().Tag(NormalizedLandmarks).IsEmpty() && sendConditionMet()){
                cc->Outputs().Tag(NormalizedLandmarks).AddPacket(cc->Inputs().Tag(NormalizedLandmarks).Value());
            }
            return ::mediapipe::OkStatus();
        }
        private:
        virtual bool sendConditionMet() = 0;
    };

    class CoordinateFrameLimitationCalculator : public LimitationCalculator {
        public:
        CoordinateFrameLimitationCalculator(){};
        ~CoordinateFrameLimitationCalculator(){};

        private:
        bool sendConditionMet(){
            if(ticks_to_send <= 0){
                ticks_to_send = SEND_EVERY_X_FRAMES;
                return true;
            }
            ticks_to_send--;
            return false;
        }
        int ticks_to_send = 0;

    };
    REGISTER_CALCULATOR(CoordinateFrameLimitationCalculator);

    class CoordinateTimeLimitationCalculator : public LimitationCalculator {
        public:
        CoordinateTimeLimitationCalculator(){};
        ~CoordinateTimeLimitationCalculator(){};
        private:
        bool sendConditionMet(){
            auto now = time(0);
            if(now >= nextSend){
                nextSend = now + SEND_EVERY_X_SECONDS;
                return true;
            }
            return false;
        }
        time_t nextSend = time(0);

    };
    REGISTER_CALCULATOR(CoordinateTimeLimitationCalculator);
}