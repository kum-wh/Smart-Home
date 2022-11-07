import urllib
import imutils
import cv2
from keras.models import load_model
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from django.core.mail import send_mail
from .models import *
import numpy as np
from PIL import Image
from django.views.decorators.csrf import csrf_exempt
from email.mime.image import MIMEImage
from django.core.mail import EmailMultiAlternatives

emotion_model = load_model("model/emotions_v4.hd5")

hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

# Create your views here.

def index(request):
    if request.method == "POST":
        number = request.POST.get('rnumber')
        state = State.objects.get(id=1)
        state.number = number
        state.save()
    return render(request, "index.html")


def emotion(request):
    if request.method == "POST":
        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("emotion.jpg","wb")
        output.write(data)
        output.close()

        emotion_dict = {0: 'angry', 1: 'happy', 2: 'neutral'}
        image = Image.open('emotion.jpg')
        image = image.convert('L')
        image = image.resize((48, 48), resample=Image.LANCZOS)
        img_array = np.array(image) / 255.0
        final_array = np.expand_dims(img_array, axis = 0)
        image = image.save('emotion.jpg', quality=95)
        
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

        state = State.objects.get(id=1)
        state.emotion = max_class
        state.save()
        
    return render(request, "emotion.html")


def security(request):
    if request.method == "POST":

        state = State.objects.get(id=1)
        expected_humans = state.number 
        print("Expected humans: %d" % expected_humans)

        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("security.jpg","wb")
        output.write(data)
        output.close()

        image = cv2.imread("security.jpg")
        image = imutils.resize(image, width = min(800, image.shape[1]))

        bounding_box_coordinates, weights = hog.detectMultiScale(image, winStride = (4, 4),
                padding = (8, 8), scale = 1.03)

        person = 0
        for x, y, w, h in bounding_box_coordinates:
            image = cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)
            image = cv2.putText(image, f'person {person}', (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,
                0, 255), 1)
            person += 1

        cv2.imwrite('security.jpg', image)
        
        print("Humans detected: %d" % person)

        body_html = '''
        <html>
            <body>
                <p>Hi User,
                <br>
                <br>An intruder has been captured by your security camera. Please take necessary action as soon as possible.
                <br>
                <img src="cid:security.jpg" />
                <br>
                </p>
            <body>
        </html>
        '''

        if person > expected_humans:
            message = EmailMultiAlternatives(
                    'INTRUDER ALERT',
                    body_html,
                    from_email = 'noreply.smarthome3237@gmail.com',
                    to = ['pzhixiang.99@gmail.com']
            )
            message.mixed_subtype = 'related'
            message.attach_alternative(body_html, "text/html")
            img_dir = 'static'
            image = 'security.jpg'
            file_path = os.path.join(img_dir, image)
            with open(file_path, 'r') as f:
                img = MIMEImage(f.read())
                img.add_header('Content-ID', '<{name}>'.format(name = image))
                img.add_header('Content-Disposition', 'inline', filename = image)
                message.attach(img)

            message.send()
            print("Intruder Alert")

        '''
        number = model.predict(image)
        state = State.objects.get(id=0)
        rnumber = state.number
        if rnumber > state.number:
            raise flag
        '''

        state.humans = person
        state.save()

    return render(request, "security.html")


def getemo(request):
    
    state = State.objects.get(id=1)
    value = state.emotion
    
    # value = 0
    return HttpResponse(f'{value}')

@csrf_exempt
def slave(request):
    state = State.objects.get(id=1)
    slave = state.slave

    if request.method == "POST":
        slave = request.body
        state.slave = slave
        state.save()

    return HttpResponse(f'{slave}')

