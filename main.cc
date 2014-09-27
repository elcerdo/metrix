#include <Ogre.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{

    Ogre::Root* root = new Ogre::Root();
		cout << "prout" << endl;

    if (!root->showConfigDialog()) return -1;

		return 0;
}

