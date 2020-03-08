# SigNN

## Abstract

The goal of SigNN is to develop a software which is capable of real-time translation of American Sign Langauge (ASL) into English text. Due to various contraints, the scope has been limited to completing reliable translation for 24 of the 26 characters of the alphabet. We achieve this objective using Mediapipe: an alpha-stage framework for building multimodal, cross platform, applied ML pipelines. All data used to train our neurall network was self-collected, aggregated, and analyzed through the use of scripts written in Google Colab.

## Contributors

- Arian Alavi
- Vahe Barseghyan
- Rafael Trinidad
- Gokul Deep
- Kenny Yip
- John Kirchner
- Daniel Lohn
- Conor O'Brien

## Motivation

Developing a tool for sign language translation has been a project many have attempted in the past two decades. A primitive start to the general project of sign language translation was the sign language glove. [In 2002, a MIT student was amongst the first to achive a basic form of translation through the use of a glove.](https://lemelson.mit.edu/resources/ryan-patterson "The system includes a soft, leather glove outfitted with ten sensors that a signer wears on his or her hand, and a small computer that associates each hand position with a corresponding letter. By finger-spelling words using the standard American Sign Language alphabet, each letter would be transmitted to and captured by the processing unit") Since then, many different types of sign langauge golve prototypes have been made. However, gloves have many limitations. One obvious limitation is the impractcality of carrying around such a glove at all times. One less obvious limitation is that sign langauge involves just more than the hands - it involves arm movements and facial expression as well.

As AI and computation power have improved over time, people have attempted to develop a tool for sign language translation through the use of computer vision. Althoguh some attempted to interpertate pixles for sign language translation, this has been found to be very ineffective. The path that is most often taken in the field of sign language is to work with a framework that provides you with an accurate represntation of hands given an image. This simplifies the problem greatly - from "Given an image, what am I signing?" to "Given a series of coordinates, what am I signing?"

There have been a few different frameworks created to detect the position of hands, but the most famous of them is of course, OpenPose. OpenPose not only interprets the hands of an image, but also the arms and face, which are other vital parts of sign langauge translation.

![](https://user-images.githubusercontent.com/49175620/76171514-8e87e900-6149-11ea-8c7e-963cc5c74173.gif)

As impressive as OpenPose is, it faces one major challenge: speed. It is extremely slow and cannot be run on the average smartphone or computer in the forseeable future. By the developers' own measurements, [the model runs at .1 FPS to .3 FPS average depending on the OpenPose model used. ](https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/speed_up_openpose.md "The CPU version runs at about 0.3 FPS on the COCO model, and at about 0.1 FPS (i.e., about 15 sec / frame) on the default BODY_25 model."). Such speed is nowhere near sufficient to enable real-time sign language translation. Prehaps if it is run in a powerful, remote server it could work. However, such a server would have an unmaintanably expensive cost per user ratio unless the users would be prepared to pay a monthly subscription. Without the issue of speed, Openpose has been shown to be an effective framework for sign langauge translation and the favorite choice of researchers.

In June 2019, developers at Google first created the repo Mediapipe on GitHub. Mediapipe, unlike OpenPose, is not a just a neural network but is an entire framework for the management and executing of neural networks on streams of data (such as video and audio). Mediapipe is still in alpha stages and lacks much comprehensive documentation. Compared to OpenPose, it should be less effective for sign langauge translation given that the provided model only detects hands and not the entire arm plus face. However, what makes Mediapipe, in practice, more effective than Openpose given its impressive framerate and ability to be run on any enviornment: from computer to smarthpone [to even browser.](https://viz.mediapipe.dev/runner/demos/hand_tracking/hand_tracking.html "Click here for a Mediapipe hand tracking demo")

![](https://raw.githubusercontent.com/google/mediapipe/master/mediapipe/docs/images/mobile/hand_tracking_android_gpu.gif)

Sign language translation has yet to be done comprehensively with Mediapipe, given that it is a brand new and yet-to-be documented technology. Our motivation for this project is **to be among the first to use this promising new technology and apply it to a problem that many have attempted to tackle in the past.**

## Methodology

When we began work on our project we had Mediapipe. Mediapipe provided a solid foundation for both data collection as well as hand tracking. This meant that we already had a major obstacle removed, hand tracking, which meant we didn't have to waste time creating our own hand tracking system. There was a tradeoff, Mediapipe was new and barely documented, meaning we had to learn how it worked with almost no resources to look to, not even StackOverflow. We eventually learned enough about Mediapipe to get it working and started work on some algorithms as well as a neural network to turn coordinates into letters.

When we returned from Winter Break we not only had 2 working algorithms but a neural network that was accurate 87% of the time. We knew the algorithms were going to become obsolete within a few weeks of their completion, but they provided insight into potential obstacles that the neural network may encounter.

For example, some letters were acidentally detected more often
than others such as M or I due to sharing characteristics with multiple other letters.

![alt text](https://github.com/JohnK-mi/mediapipe/blob/master/scripts/mediapipe_image_means/M.png)

![alt text](https://github.com/JohnK-mi/mediapipe/blob/master/scripts/mediapipe_image_means/I.png)

## Key Results

With the data we currently have and the neural network we are currently using we have an accuracy hovering around 80%
As of right now this only applies to 24 letters of the alphabet (J and Z were excluded due their motion based nature)
This is impressive as it doesn't require any external teacking systems, only a camera or webcam is necessary to begin understanding ASL.
While it is limited now, what we have created provides a solid foundation for addition of new letters and even words.
And with even more data our accuracy could improve further, as currently our data consists solely of our own team members hands.
With more time, effort, and resources our project could become a very important part of communication for ASL users.

## Summary

We have poured a lot of time and effort into a project we hope can serve to further communication between ASL users and non-ASL users.
We learned a lot regarding ASL, hand-tracking, neural networks, data collection, and most of all teamwork.
This entire project would not have been possible without the help of every single one of our team members.

## Future Work

We plan to add words in the future as well as J and Z characters, all of which are motion based rather than static. Even though it serves a greater challenge we believe that we will be able to tackle it.
Another portion of our future plan is to collect more data, both to increase the accuracy of the 24 letters as well as to allow us to extend into words.
