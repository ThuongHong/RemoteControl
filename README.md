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
    * Click **Configure**.
    * After the first configuration, use the search box at the top to look for the following option: ```BUILD_SHARED_LIBS```: Make sure this is unchecked to disable shared library building.
    * After setting the options, click **Configure again** if prompted to apply changes. Once configuration is complete and there are no errors, click **Generate** to create the build files.

* Build the Library:
   * Open a terminal or command prompt, navigate to your build directory, and run the following command:
     ```bash
     cmake --build . --config Release
     ```

## wxWidgets
* Download wxWidgets from [wxWidgets Downloads](https://www.wxwidgets.org/downloads/).


* Unzip the folder and place it in a location of your choice.
* Set up the wxWidgets path in Windows environment variables:
    * Open **Environment Variables**.
    * In **System variables**, click **New**:
        * **Varialbe name**: WXWIN
        * **Variable value**: ```<your_path>\WxWidget```
    * Confirm by clicking OK to save changes.

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
