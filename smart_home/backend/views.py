import urllib
# import cv2
from keras.models import load_model
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from .models import *
import numpy as np
from PIL import Image

emotion_model = load_model("model/emotions_v2.hd5")
# security_model = load_model()

# Create your views here.

def index(request):
    if request.method == "POST":
        number = request.POST.get('rnumber')
        state = State.objects.get(id=0)
        state.number = number
    return render(request, "index.html")


def emotion(request):
    if request.method == "POST":
        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("emotion.jpg","wb")
        output.write(data)
        output.close()

        emotion_dict = {0: 'angry', 1: 'happy', 2: 'neutral', 3: 'sad'}
        image = Image.open('emotion.jpg')
        image = image.convert('L')
        image = image.resize((48, 48), resample=Image.LANCZOS)
        img_array = np.array(image) / 255.0
        final_array = np.expand_dims(img_array, axis = 0)
        image = image.save('emotion.jpg', quality=95)

        # image =  cv2.imread("emotion.jpg")
        
        result = emotion_model.predict(final_array)
        max_class = np.argmax(result)
        print("Detected %s with confidence %3.2f" % (emotion_dict[max_class], result[0][max_class]))

        '''
        if max_class == "angry":
            colour = 1"purple"
        elif max_class == "happy":
            colour = 2 "blue"
        elif max_class == "neutral":
            colour = 
        else:
            colour == 0 "white"
        '''

        # state = State.objects.get(id=0)
        # state.emotion = emotion_dict[themax]
        
    return render(request, "emotion.html")


def security(request):
    if request.method == "POST":
        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("security.jpg","wb")
        output.write(data)
        output.close()
        # image =  cv2.imread("security.jpg")
        '''
        number = model.predict(image)
        state = State.objects.get(id=0)
        rnumber = state.number
        if rnumber > state.number:
            raise flag
        '''
    return render(request, "security.html")


def getemo(request):
    '''
    state = State.objects.get(id=0)
    value = state.emotion
    '''
    value = 0
    return HttpResponse(f'{value}')
