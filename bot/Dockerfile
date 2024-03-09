FROM python:3.9.6

ADD . .

RUN apt-get update
RUN apt-get install -y cmake
WORKDIR ./engines/jchess_engine2/cmake-build-release
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN cmake --build . --target jchess_engine
WORKDIR ../../../
RUN pip install --no-cache-dir -r requirements.txt
CMD [ "python", "./lichess-bot.py" ]