main:
	sudo apt-get install g++ cmake libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev  libxi-dev libfreetype-dev libgl1-mesa-dev xorg-dev
	cmake -S . -B build
	cd build && make && clear && ./grafika3d-projekt1