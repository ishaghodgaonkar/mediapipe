#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/calculators/tensorflow/tensorflow_session.h"

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/framework/tensor_util.h"

#if !defined(__ANDROID__) && !defined(__APPLE__)
#include "tensorflow/core/profiler/lib/traceme.h"
#endif



#include <vector>
#include <string>
#include <iostream>

namespace tf = ::tensorflow;

namespace mediapipe{

    namespace{
        constexpr char NormalizedLandmarks[] = "LANDMARKS";
    }


    class SigNNCalculator : public CalculatorBase {
        public:
        SigNNCalculator(){};
        ~SigNNCalculator(){};

        static ::mediapipe::Status GetContract(CalculatorContract* cc){
            cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();
            cc->InputSidePackets().Tag("SESSION").Set<TensorFlowSession>();
            if (cc->InputSidePackets().HasTag("RECURRENT_INIT_TENSORS")) {
            cc->InputSidePackets()
                .Tag("RECURRENT_INIT_TENSORS")
                .Set<std::unique_ptr<std::map<std::string, tf::Tensor>>>();
            }
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Open(CalculatorContext* cc){
            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Process(CalculatorContext* cc){
            std::vector<std::vector<NormalizedLandmark>> hands = cc -> Inputs().Tag(NormalizedLandmarks).Get<std::vector<std::vector<NormalizedLandmark>>>();

            return ::mediapipe::OkStatus();
        }
        ::mediapipe::Status Close(CalculatorContext* cc){
            return ::mediapipe::OkStatus();
        }

        private:

    };
    REGISTER_CALCULATOR(SigNNCalculator);
}