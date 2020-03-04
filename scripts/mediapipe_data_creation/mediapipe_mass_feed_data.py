import os
import sys
import json

from PIL import Image, ImageOps
import cv2
import uuid
import time
import threading
from queue import *

# Instructions:
# 1. Make sure mediapipe_feed_data.py is in same directory
# 2. Put all images in folders in current directory, name folder after what image represents
# 3. run "python3 mediapipe_mass_feed_data.py"

# These parameters may be set by human
OUTPUT_FILE = "training_data.json"
CONCURRENT_MEDIAPIPE_HAND_CHECKER_THREADS = 8
BASE_RESOLUTION = (1920, 1920)


# Below parameters are set by script
SCRIPT_PATH = None 


def setResolution(path, resolution):
    assert isinstance(path, str), "Is actually {} with value {}".format(type(path), path)
    assert isinstance(resolution, tuple)
    assert len(resolution) == 2
    im = Image.open(path)
    if im.size == resolution:
        return
    im = ImageOps.fit(im, resolution)
    im.save(path, dpi=resolution)

def runMediapipe(filename, mediapipe_directory, outputname):
    command = "python mediapipe_feed_data.py --input={} --mediapipe_directory={}".format(filename, mediapipe_directory)
    # print("COMMAND:", command)
    stream = os.popen(command)
    output = stream.read()
    if output:
        result = json.loads(output)
        return [x for x in result[1::2] if any(y for y in x)]
    return []

class TrainingImage():
    def __init__(self, path):
        assert isinstance(path, str)
        self.path = path

    def testBad(self, primer, mediapipeDirectory, absolutePath):
        output_file = str(uuid.uuid4()) + ".avi"
        output_file = os.path.join(absolutePath, output_file)
        testVideo = createVideoFromImages([self,], output_file, primer)
        result = runMediapipe(testVideo, mediapipeDirectory, output_file)
        os.remove(output_file)
        if len(result) != 1:
            print("{} had {} hands in it".format(self.path, len(result)))
            self.destroy()
            return True
        return False
    def destroy(self):
        print("{} destroyed".format(self.path))
        os.remove(self.path)
        del self

    def __hash__(self):
        return int(os.path.getmtime(self.path) * 10000000)

    def __str__(self):
        return self.path

    def __repr__(self):
        return self.path
        
def checkAlreadyRenamed(file_name):
    def checkValidUUID(givenStr):
        assert isinstance(givenStr, str)
        try:
            uuid.UUID(givenStr)
            return True
        except ValueError:
            return False

    assert isinstance(file_name, str)
    try:
        file_name = os.path.splitext(file_name)[0]
        if any(checkValidUUID(x) for x in file_name.split("_")):
            return True
        return False
    except Exception as e:
        print("Check process exception:", e)
        return False

def renameImageUUID(absolutePath, file_name, forName):
    if checkAlreadyRenamed(file_name) or os.path.isdir(file_name):
        return file_name
    newname = forName + "_" + str(uuid.uuid4()) + os.path.splitext(file_name)[1]
    os.rename(os.path.join(absolutePath, file_name), os.path.join(absolutePath, newname))
    return newname

def renameImagesUUID(absolutePath, forName):
    # for folder in os.listdir(absolutePath):
    #     if os.path.isdir(os.path.join(absolutePath, folder)):
    for file in os.listdir(absolutePath):
        if file.endswith(".jpg") or file.endswith(".jpeg") or file.endswith(".png"):
            renameImageUUID(absolutePath, file, forName)

def renameAll():
    for folder in os.listdir(os.getcwd()):
        if os.path.isdir(os.path.join(os.getcwd(), folder)):
            renameImagesUUID(os.path.join(os.getcwd(), folder), folder)


def getImagesInFolder(absolutePath, forName):
    assert isinstance(absolutePath, str)
    assert isinstance(forName, str)
    image_names = [renameImageUUID(absolutePath, img, forName) for img in os.listdir(absolutePath) if img.endswith(".jpg") or img.endswith(".jpeg") or img.endswith(".png")]
    images = [TrainingImage(os.path.join(absolutePath, img)) for img in image_names]
    returnImages = []
    for img in images:
        try:
            setResolution(img.path, BASE_RESOLUTION)
            returnImages.append(img)
        except:
            print("{} is not a readable image".format(img))
            img.destroy()
    return returnImages

def createVideoFromImages(images, output_name, primer=None):
    assert isinstance(images, list)
    assert all(isinstance(x, TrainingImage) for x in images)
    assert isinstance(output_name, str)
    assert primer is None or isinstance(primer, str)
    video = cv2.VideoWriter(output_name, 0, 1, BASE_RESOLUTION)
    for image in images:
        video.write(cv2.imread(primer))
        video.write(cv2.imread(image.path))
    cv2.destroyAllWindows()
    video.release()
    return output_name


class Hash:
    HASH_FILE = "hash.txt"
    TRAINING_DATA_FILE = "training_data.json"

    @staticmethod
    def getHashFileName(word):
        return word + "_" + Hash.HASH_FILE

    @staticmethod
    def getSavedTrainingDataFileName(word):
        return word + "_" + Hash.TRAINING_DATA_FILE

    @staticmethod
    def getSavedHash(absolutePath, word):
        path = os.path.join(absolutePath, Hash.getHashFileName(word))
        try:
            file = open(path)
            result = file.read()
            file.close()
            return result
        except:
            return False

    @staticmethod
    def loadHashData(absolutePath, word):
        path = os.path.join(absolutePath, Hash.getSavedTrainingDataFileName(word))
        try:
            file = open(path)
            result = json.load(file)
            file.close()
            return result
        except:
            return None

    @staticmethod
    def getHash(images):
        assert all(isinstance(x, TrainingImage) for x in images)
        if isinstance(images, list):
            images = tuple(images)
        return hash(images)

    @staticmethod
    def saveHash(absolutePath, images, results, word):
        assert isinstance(absolutePath, str)
        assert isinstance(images, list)
        assert all(isinstance(x, TrainingImage) for x in images)
        assert isinstance(results, list)
        assert isinstance(word, str)
        newhash = Hash.getHash(images)
        path = os.path.join(absolutePath,  Hash.getHashFileName(word))
        file = open(path, "w")
        file.write(str(newhash))
        file.close()
        print("{} hash saved in {}".format(newhash, path))
        training_file = open(os.path.join(absolutePath, Hash.getSavedTrainingDataFileName(word)), "w+")
        json.dump(results, training_file)
        training_file.close()
        return newhash


def findAndDeleteBadImagesThreadPool(ALL_IMAGES, primer, mediapipeDirectory, absolutePath):
    class CheckBadWorker(threading.Thread):
        def __init__(self, jobQ, primer, mediapipeDirectory, absolutePath):
            assert isinstance(jobQ, Queue)
            self.jobQ = jobQ
            self.primer = primer
            self.mediapipeDirectory = mediapipeDirectory
            self.absolutePath = absolutePath
            self._results = []
            threading.Thread.__init__(self)

        def run(self):
            while not self.jobQ.empty():
                image = self.jobQ.get()
                if not TrainingImage.testBad(image, self.primer, self.mediapipeDirectory, self.absolutePath):
                    self._results.append(image)

        def join(self):
            threading.Thread.join(self)
            return self._results
    
    checkBadTasksQ = Queue()
    [checkBadTasksQ.put(x) for x in ALL_IMAGES]
    checkBadImageThreads = [CheckBadWorker(checkBadTasksQ, primer, mediapipeDirectory, absolutePath) for _ in range(CONCURRENT_MEDIAPIPE_HAND_CHECKER_THREADS)]
    [x.start() for x in checkBadImageThreads]
    results = [x.join() for x in checkBadImageThreads]
    return_results = []
    [return_results.extend(x) for x in results]
    return return_results

def photoToJSON(absolutePath, word, mediapipeDirectory):
    ALL_IMAGES = getImagesInFolder(absolutePath, word) # collects all images in the absolute path and renames them if they are new to take on an UUID4-based name

    savedHash =  Hash.getSavedHash(absolutePath, word)
    print("Saved hash: {} vs generated hash: {}".format(savedHash, Hash.getHash(ALL_IMAGES)))
    if str(Hash.getHash(ALL_IMAGES)) == savedHash:  # if the hash is the same, the folder may be unmodified of images
        previous_data = Hash.loadHashData(absolutePath, word) # get the training_data for the character collected last time
        print("Saved length: {} vs current length: {}".format(len(previous_data), len(ALL_IMAGES)))
        if len(ALL_IMAGES) == len(previous_data): # if the old training data is the same length as the number of images in the dataset (given that the hash is also the same) the dataset extremely likely to be unmodified
            print("{} unmodified, using previous data".format(word))
            return previous_data
        else:
            print("{} hash found, but since has been modified by {}".format(len(ALL_IMAGES) - len(previous_data)))


    output_file = word + ".avi" # output_file will be the stiched together images
    output_file = os.path.join(absolutePath, output_file) # most paths must be absolute in this script
    original_image_count = len(ALL_IMAGES)

    primer = os.path.join(SCRIPT_PATH, "primer.jpg") # adds primer to the video
    
    # ALL_IMAGES = findAndDeleteBadImagesThreadPool(ALL_IMAGES, primer, mediapipeDirectory, absolutePath) # deletes images not detected by mediapipe
  

    # ALL_IMAGES = [x for x in ALL_IMAGES if not x.testBad(output_file, primer, mediapipeDirectory)] 
    video = createVideoFromImages(ALL_IMAGES, output_file, primer) # actually stitches images into video
    result = runMediapipe(video, mediapipeDirectory, word) # runs mediapipe on the generated video
    os.remove(output_file) # Removing video after use will greatly free up disk space
        
    if original_image_count != len(ALL_IMAGES):
        print("{} images used out of {}".format(len(ALL_IMAGES), original_image_count))
    else:
        print("{} images used".format(original_image_count))

    Hash.saveHash(absolutePath, ALL_IMAGES, result, word) # save the images as a hash for next time to check if dataset has been modified

    return result

def getMediapipeDirectory():
    MEDIAPIPE_DIRECTORY = None
    while MEDIAPIPE_DIRECTORY is None or not os.path.isdir(MEDIAPIPE_DIRECTORY):
        if MEDIAPIPE_DIRECTORY:
            print("Invalid directory!")
        MEDIAPIPE_DIRECTORY = input("Please enter mediapipe absolute directory:\n")  
        # if not os.path.isabs(MEDIAPIPE_DIRECTORY):
        #     MEDIAPIPE_DIRECTORY = None
        #     print("Must be an absolute path!")
        if MEDIAPIPE_DIRECTORY and MEDIAPIPE_DIRECTORY[-1] != "/":
            MEDIAPIPE_DIRECTORY += "/"      
    return MEDIAPIPE_DIRECTORY

def sudoActivator():
    stream = os.popen("sudo echo 'Sudo mode activated'")
    output = stream.read()
    return True

def main():
    global SCRIPT_PATH
    sudoActivator()
    pathname = os.path.dirname(sys.argv[0]) 
    SCRIPT_PATH = os.path.abspath(pathname)

    MEDIAPIPE_DIRECTORY = getMediapipeDirectory() # prompts user for mediapipe directory
    
    CURRENT_PATH = os.getcwd()
    RESULTS = {}
    for file in os.listdir(CURRENT_PATH): # get all subfolders in the current running directory
        full_path = os.path.join(CURRENT_PATH, file)
        if os.path.isdir(full_path) and "__pycache__" not in file:
            print("{} started".format(file))
            RESULTS[file] = photoToJSON(full_path, file, MEDIAPIPE_DIRECTORY) # where everything happens
            print("{} done\n".format(file))
            

    file = open(OUTPUT_FILE, "w+")
    json.dump(RESULTS, file)


if __name__ == "__main__":
    main()