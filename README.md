# SigNN

## Abstract
Our project does real-time translation of American Sign Language into English text (characters only for now). All data was self-collected and we used google collab to collect, aggregate, and analyze data. We achieved this objective using Mediapipe, a machine learning application that can handle live streams of data.

## Contributors
- Ari Alvan
- Vahe B.
- Rafael Trinidad
- Gokul Deep
- Kenny Yip
- John Kirchner
- Daniel Lohn
- Conor O'Brien

## Motivation
There have been many attempts at creating a similar project like ours but none work very well. We knew this was going to be a difficult undertaking which is why we wanted to work on this project. We wanted to make something that would be able to help people communicate and connect with each other regardless of language barriers and sound barriers.

## Methodology
When we began work on our project we had Mediapipe. Mediapipe provided a solid foundation for both data collection as well as hand tracking. This meant that we already had a major obstacle removed, 
hand tracking, which meant we didn't have to waste time creating our own hand tracking system. 

There was a tradeoff, Mediapipe was new and barely documented, meaning we had to learn how it worked 
with almost no resources to look to, not even StackOverflow. We eventually learned enough about Mediapipe to get it working and started work on some algorithms as well as a neural network to turn 
coordinates into letters. 

When we returned from Winter Break we not only had 2 working algorithms but a neural network that was accurate 87% of the time. We knew the algorithms were going to become 
obsolete within a few weeks of their completion, but they provided insight into potential obstacles that the neural network may encounter.

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





