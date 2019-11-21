#!/bin/bash

sudo GLOG_logtostderr=1 LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libv4l/v4l2convert.so bazel-bin/mediapipe/examples/desktop/multi_hand_tracking/multi_hand_tracking_cpu --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_desktop_live.pbtxt
