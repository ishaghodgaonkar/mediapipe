# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:private"])

load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library")


cc_library(
    name = "python_caller_calculator",
    srcs = ["python_caller_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/deps:file_path",
        "@python_linux//:python36-lib",
    ] + select({
        "//conditions:default": [
            "//mediapipe/framework/port:file_helpers",
        ],
    }),
    data = ["main.py"],
    alwayslink = 1,
)

cc_library(
    name = "logger_calculator",
    srcs = ["coordinate_logger.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/deps:file_path",
    ] + select({
        "//conditions:default": [
            "//mediapipe/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "console_logger_calculator",
    srcs = ["coordinate_console_logger.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/deps:file_path",    
    ],
    alwayslink = 1
)

cc_library(
    name = "coordinate_modifiers",
    srcs = ["coordinate_modifiers.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:location_data_cc_proto",
        "//mediapipe/framework/formats:detection_cc_proto",
    ],
    alwayslink = 1
)