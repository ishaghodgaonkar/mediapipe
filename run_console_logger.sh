#!/bin/bash

sudo GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/multi_hand_tracking/multi_hand_tracking_cpu --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_desktop_console_logger.pbtxt --input_video_path=/home/ari/Downloads/L_TEST.mp4