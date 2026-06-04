# DOCKER build and run

sudo docker build -t ros2-jazzy-dev .

sudo docker run --rm -it   --network host   -v $(pwd):/workspace   ros2-jazzy-dev

all further comands needed to build and run are in "RUN" file
