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
const int ROLLING_AVERAGE_WINDOW_FRAMES = 3;

namespace mediapipe{

    namespace{
        constexpr char NormalizedLandmarks[] = "LANDMARKS";
    }

    // This calculator is an abstract class, don't pay too much attention to it
    class LimitationCalculator : public CalculatorBase {
        public:
        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            cc->Outputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Process(CalculatorContext* cc){
            if(! cc->Inputs().Tag(NormalizedLandmarks).IsEmpty() && sendConditionMet()){
                cc->Outputs().Tag(NormalizedLandmarks).AddPacket(cc->Inputs().Tag(NormalizedLandmarks).Value());
            }
            return ::mediapipe::OkStatus();
        }
        private:
        virtual bool sendConditionMet() = 0;
    };

    // This calculator will send only every 'x' frame through it. For example, will send only every 3rd frames. It does not care how long it has been since the last frame
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

    // This calculator will send a frame every 'x' seconds. It doesn't care if 1 frame or 10 frames have passed within the specified interval
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

    // This calculator will spit out the average frame of the last x frames
    // This calculator will not have output until the specified x number of frames are seen
    class RollingAverageCalculator : public CalculatorBase {
        public:
        RollingAverageCalculator(){};
        ~RollingAverageCalculator(){};
        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            cc->Outputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            return ::mediapipe::OkStatus();
        }

        ::mediapipe::Status Process(CalculatorContext* cc){
            std::vector<std::vector<NormalizedLandmark>> hands = cc -> Inputs().Tag(NormalizedLandmarks).Get<std::vector<std::vector<NormalizedLandmark>>>();

            if(averagePointer >= ROLLING_AVERAGE_WINDOW_FRAMES){
                averagePointer = 0;
                full = true;
            }
            if(hands.size() > 0){
                memory[averagePointer] = hands.at(0);
                averagePointer++;
            }

            if(full){
                std::vector<NormalizedLandmark> result = getAverage();
                std::vector<std::vector<NormalizedLandmark>> send;
                send.push_back(result);
                std::unique_ptr<std::vector<std::vector<NormalizedLandmark>>> output_stream_collection = std::make_unique<std::vector<std::vector<NormalizedLandmark>>>(send);
                cc -> Outputs().Tag(NormalizedLandmarks).Add(output_stream_collection.release(), cc->InputTimestamp());
            }
            return ::mediapipe::OkStatus();
        }        
        private:
        std::vector<NormalizedLandmark> memory[ROLLING_AVERAGE_WINDOW_FRAMES];
        int averagePointer = 0;
        bool full = false;



        std::vector<NormalizedLandmark> getAverage(){
            std::vector<NormalizedLandmark> result;
            float averages[42] = {};
            for(int savedInstance = 0; savedInstance < ROLLING_AVERAGE_WINDOW_FRAMES; savedInstance++){
                for(int pointIndex = 0; pointIndex < 21; pointIndex+= 1){
                    averages[pointIndex * 2] += memory[savedInstance].at(pointIndex).x() / ROLLING_AVERAGE_WINDOW_FRAMES;
                    averages[pointIndex * 2 + 1] += memory[savedInstance].at(pointIndex).y() / ROLLING_AVERAGE_WINDOW_FRAMES;

                }
            }
            for(int i = 0; i < 42; i+= 2){
                NormalizedLandmark l = NormalizedLandmark();
                l.set_x(averages[i]);
                l.set_y(averages[i + 1]);
                result.push_back(l);
            }
            return result;
        }

    };
    REGISTER_CALCULATOR(RollingAverageCalculator);
}