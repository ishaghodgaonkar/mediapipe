


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

#include "Python.h"

#include<signal.h>

void handler() {printf("Exiting main.."); exit(0);}

namespace mediapipe{

namespace{
    constexpr char NormalizedLandmarks[] = "LANDMARKS";
    constexpr char kDetectionsTag[] = "DETECTIONS";


}

    class SignnCalculator : public CalculatorBase {

    public:
    SignnCalculator() {}
    ~SignnCalculator() {}


    static ::mediapipe::Status GetContract(CalculatorContract* cc){
        cc->Inputs().Tag(NormalizedLandmarks).Set<std::vector<std::vector<NormalizedLandmark>>>();

        return ::mediapipe::OkStatus();
    }

    ::mediapipe::Status Open(CalculatorContext* cc){
        Py_InitializeEx(0);

        PyRun_SimpleString("import sys; import os; sys.path.append(os.getcwd()); print(os.getcwd());");
        std::string python_main_path = mediapipe::file::JoinPath("./",
                                                                "mediapipe/calculators/signn/");
        std::cout << "Python main path: " << python_main_path << "\n";

        std::string add_to_path = "import sys; sys.path.insert(0, '" + python_main_path + "')";
        PyRun_SimpleString(add_to_path.c_str());

        PyObject* module_string = PyUnicode_FromString((char*)"main");
        
        python_module = PyImport_Import(module_string);      
        PyErr_Print();
        PyObject* main_python_loaded = PyObject_GetAttrString(python_module, (char*)"testLoaded");
        
        RET_CHECK_EQ(PyObject_IsTrue(main_python_loaded), true);
        python_point = PyObject_GetAttrString(python_module, (char*)"Point");
        python_hand = PyObject_GetAttrString(python_module, (char*)"Hand");
        return ::mediapipe::OkStatus();
        
    }

    ::mediapipe::Status Close(){
        std::cout << "Close";
        Py_Finalize();
        return ::mediapipe::OkStatus();
    }

    ::mediapipe::Status Process(CalculatorContext* cc){
        std::vector<std::vector<NormalizedLandmark>> hands = cc -> Inputs().Tag(NormalizedLandmarks).Get<std::vector<std::vector<NormalizedLandmark>>>();
        int number_of_hands = hands.size();

        for(int i = 0; i < number_of_hands; i++){
            std::vector<NormalizedLandmark> coordinates = hands.at(i);
            PyObject* python_coordinates_tuple = PyTuple_New(21);
            for(int j = 0; j < coordinates.size(); j++){

                PyObject* python_int_x = PyFloat_FromDouble(coordinates.at(j).x());
                PyObject* python_int_y = PyFloat_FromDouble(coordinates.at(j).y());
                PyObject* python_x_y = PyTuple_New(2);

                PyTuple_SET_ITEM(python_x_y, 0, python_int_x);
                PyTuple_SET_ITEM(python_x_y, 1, python_int_y);
                
                PyObject* python_point_instance = PyObject_CallObject(python_point, python_x_y);
                PyTuple_SET_ITEM(python_coordinates_tuple, j, python_point_instance);

                
            }
            PyErr_Print();
            PyObject_CallObject(python_hand, python_coordinates_tuple);
            PyErr_Print();
        }

        return ::mediapipe::OkStatus();
    }

    private:
        PyObject* python_module;
        PyObject* python_point;
        PyObject* python_hand;


    };

    REGISTER_CALCULATOR(SignnCalculator);

}