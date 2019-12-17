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
#include <stdexcept>
#include <regex>


std::vector<float> getFloats(std::string basestring){
    std::vector<float> results;
    std::regex numberlist("([0-9.]+)");
    for(auto i = std::sregex_iterator(basestring.begin(), basestring.end(), numberlist); i != std::sregex_iterator(); i++){
        results.push_back(std::stof(i -> str()));
    }
    return results;
}

namespace mediapipe{

    namespace{
        constexpr char NormalizedLandmarks[] = "LANDMARKS";
    }


    class ErrorMinCalculatorAlgoCalculator : public CalculatorBase {
        public:
        ErrorMinCalculatorAlgoCalculator(){};
        ~ErrorMinCalculatorAlgoCalculator(){};

        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Open(CalculatorContext* cc){
            std::string data_path = mediapipe::file::JoinPath("./",
                                                            "mediapipe/calculators/signn/");
            const std::string DATA_FILENAME = "mean_data.txt";
            data_path = mediapipe::file::JoinPath(data_path, DATA_FILENAME);
            LOG(INFO) << "Opening training data at the following path: " << data_path;
            std::ifstream datafile;
            std::string data;
            datafile.open(data_path);
            int line = 0;
            bool none = false;
            
            while(std::getline(datafile, data)){
                switch(line){
                    case 0:
                        sample_words.push_back(data.substr(6, data.length()));
                        break;
                    case 1:
                        if(data == "None"){
                            none = true;
                            sample_words.pop_back();
                        }else{
                            sample_means.push_back(getFloats(data));
                        }                       
                        break;
                    case 2:
                        if(! none){
                            sample_stdevs.push_back(getFloats(data));
                        }
                        break;
                    case 3:
                        if(! none){
                            double size = std::stod(data.substr(3, data.length()));
                            if(size == 0){
                                sample_words.pop_back();
                                sample_means.pop_back();
                                sample_stdevs.pop_back();
                            }else{
                                sample_sizes.push_back(size);
                            }
                            
                        }
                        
                        break;
                    default:
                        throw std::logic_error("Get line loop should not take on any other values");
                }
                if(line == 3){
                    line = 0;
                    none = false;
                }else{
                    line++;
                }
            }
            LOG(INFO) << "data found: " << data;
            datafile.close();
            if(sample_words.size() != sample_means.size()){
                return ::mediapipe::InternalError("Invalid data");
            }


            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Process(CalculatorContext* cc){
            std::vector<std::vector<NormalizedLandmark>> hands = cc -> Inputs().Tag(NormalizedLandmarks).Get<std::vector<std::vector<NormalizedLandmark>>>();



            for(int i = 0; i < hands.size(); i++){
                auto hand = hands.at(i);
                std::vector<float> coordinates;
                for(int j = 0; j < hand.size(); j++){
                    coordinates.push_back(hand.at(j).x());
                    coordinates.push_back(hand.at(j).y());
                }
                result r = errorMin(coordinates);
                LOG(INFO) << "( " << std::to_string(r.error) << " error ) " << r.name;

            }
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Close(CalculatorContext* cc){
            return ::mediapipe::OkStatus();
        }

        private:
        std::vector<std::string> sample_words;
        std::vector<std::vector<float>> sample_means;
        std::vector<std::vector<float>> sample_stdevs;
        std::vector<double> sample_sizes;

        float errorTest(const std::vector<float> coordinates, const std::vector<float> training_mean_coordinates, const std::vector<float> training_stdev, const double training_sample_size){
            float error = 0;
            for(int i = 0; i < training_mean_coordinates.size(); i++){
                float top = std::ldexp((coordinates.at(i) - training_mean_coordinates.at(i)), 2);
                if(top < 0){
                    top *= -1;
                }
                // error += top / (training_stdev.at(i) / std::sqrt(training_sample_size)); When stdev / sqrt(n) (STANDARD ERROR) is thrown in, V and G become the only guesses
                
            }
            return error;
        };
        struct result{
            float error;
            std::string name;
        };

        result errorMin(std::vector<float> foundData){
            float min_error = std::numeric_limits<float>::max();
            std::string best_word = "";

            for(int i = 0; i < sample_words.size(); i++){
                float foundError = errorTest(foundData, sample_means.at(i), sample_stdevs.at(i), sample_sizes.at(i));
                if(foundError < min_error){
                    min_error = foundError;
                    best_word = sample_words.at(i);

                }
            }
            return result{
                min_error,
                best_word
            };
        }


    };
    REGISTER_CALCULATOR(ErrorMinCalculatorAlgoCalculator);
}