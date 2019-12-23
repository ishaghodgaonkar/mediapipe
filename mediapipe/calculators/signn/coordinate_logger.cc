#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/deps/file_path.h"
#include "mediapipe/framework/port/file_helpers.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>   

namespace mediapipe{

    namespace{
        constexpr char NormalizedLandmarks[] = "LANDMARKS";
    }


    class CoordinateLoggerCalculator : public CalculatorBase {
        public:
        CoordinateLoggerCalculator(){};
        ~CoordinateLoggerCalculator(){};

        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Open(CalculatorContext* cc){
            std::string log_path = mediapipe::file::JoinPath("./",
                                                            "mediapipe/calculators/signn/log/");
            auto now = std::chrono::system_clock::now();
            time_t now_as_time = std::chrono::system_clock::to_time_t(now);
            std::string time_as_string = (std::string) ctime(&now_as_time);
            // time_as_string = Fri Nov 29 01:33:30 2019
            std::string fileName = "";
            bool first_space_hit = false;
            for(char c: time_as_string){
                if(first_space_hit){
                    if(c == ' '){
                        fileName += '_';
                    }else{
                        fileName += c;
                    }
                }
                if(c == ' '){
                    first_space_hit = true;
                }
            }
            // fileName = Nov_29_01:33:30_2019
            fileName += ".json";
            // fileName = Nov_29_01:33:30_2019.txt
            log_path = mediapipe::file::JoinPath(log_path, fileName);
            LOG(INFO) << "Opening logger at the following path: " << log_path;
            file.open(log_path);
            file << "[";


            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Process(CalculatorContext* cc){
            std::vector<std::vector<NormalizedLandmark>> hands = cc -> Inputs().Tag(NormalizedLandmarks).Get<std::vector<std::vector<NormalizedLandmark>>>();
            if(firstwrite){
                file << "[";
                firstwrite = false;
            }else{
                file << ", [";
            }

            for(int i = 0; i < hands.size(); i++){
                auto hand = hands.at(i);
                file << "[";
                for(int j = 0; j < hand.size(); j++){
                    file << std::fixed << hand.at(j).x() << ", " << std::fixed << hand.at(j).y();
                    if(j + 1 < hand.size()){
                        file << ", ";
                    }
                }
                if(i + 1 < hands.size()){
                    file << "],";
                }else{
                    file << "]";
                }
                

            }
            file << "]";
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Close(CalculatorContext* cc){
            LOG(INFO) << "Closing logger";
            file << "]";
            file.close();
            return ::mediapipe::OkStatus();
        }

        private:
        bool firstwrite = true;
        std::ofstream file;

    };
    REGISTER_CALCULATOR(CoordinateLoggerCalculator);
}