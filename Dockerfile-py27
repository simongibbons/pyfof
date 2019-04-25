FROM python:2.7

COPY . /app

WORKDIR /app

RUN apt-get update && apt-get install libboost1.62-dev

RUN pip install -r requirements.txt

RUN python setup.py bdist_wheel
