#include "inc.h"

typedef int tEntry;

struct Entry {
    bool isSuccessfull;
    bool getState() { return !isSuccessfull; }
};

#include "render.h"

tEntry wmain() {
    try {
		Gui.vsync = false;
        // Replace "Counter-Strike 2" with your game / program title
		Gui.AttachAnotherWindow("Counter-Strike 2","", DrawCallBack);
	}
	catch (OSImGui::OSException& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}