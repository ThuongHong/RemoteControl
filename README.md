# Remote Control using Gmail

## Prerequisites
* **C++20**
* **CMake 3.24** or later

**Libraries for Client:**
* curl
* nlohmann/json
* wxWidgets

**Libraries for Server:**
* GDI+
* OpenCV

## Installation
First, clone this repo:
```bash 
git clone https://github.com/ThuongHong/RemoteControl.git
```
# Client
## curl
* Clone the CURL repository:
```bash
git clone https://github.com/curl/curl.git 
``` 
* Move the cloned CURL repo to ```client/out/build```.
* In ```client/out/build/curl```, create a build directory and navigate into it:
```bash
mkdir build && cd build
```
* Open CMake GUI
    * **"Where is the source code"**: Select the main ```curl``` folder.
    * **"Where to build the binaries"**: Select the the ```curl/build``` dỉrectory.
    * Click **Configure**, then **Generate**, and **Close** once completed.

## wxWidgets
* Download wxWidgets from [wxWidgets Downloads](https://www.wxwidgets.org/downloads/).


* Unzip the folder and place it in a location of your choice.

# Server
## OpenCV
* Download OpenCV from [OpenCV Releases](https://opencv.org/releases/).
* Unzip the folder and place it in a suitable location.
* Set up the OpenCV path in Windows environment variables:
    * Open **Environment Variables**.
    * Under **Path**, click **New** and add:
        * ```<your_path>\opencv\build\x64\vc16\bin```
        * ```<your_path>\opencv\build\x64\vc16\lib```
    * Confirm by clicking OK to save changes.

### Run code (In Visual Studio 2022):
* Open the ```client``` or ```server``` folder in Visual Studio.
* Open ```CMakeLists.txt```and press **Ctrl+S** to configure the project.
* Build the project:
    * **F7**: Build all.
    * **Ctrl+F5**: Run with Release configuration.
    * **Shift+F5**: Run with Debug configuration.


## License

[MIT](https://choosealicense.com/licenses/mit/)