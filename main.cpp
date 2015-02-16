#define ALLEGRO_STATICLINK

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <math.h>

#include <list>
#include <vector>

#include <allegro.h>
#include <winalleg.h>

using namespace std;

const int SCRX(800);
const int SCRY(600);

const int MAPW(40);
const int MAPH(30);
const int cs = 40; // Taille des cases

struct Node
{
    int id;
    Node * parent;
    int x;
    int y;
    int g;
    int h;
    int f;

    Node () : parent(0){}
    Node (int pX, int pY, Node * pParent = 0) : x(pX), y(pY), parent(pParent), id(y * MAPW + x), g(0), h(0) {};

    int getF() { return g+h;}
    int ManHattanDistance (Node * nodeEnd)
    {
        int x = fabs(this->x - nodeEnd->x);
        int y = fabs(this->x - nodeEnd->x);
        return x+y;
    }

    bool hasParent() { return parent != NULL;}

};


int depx(0);
int depy(0);
int arrx(0);
int arry(0);


BITMAP * buffer(NULL);

char myMap[MAPH][MAPW] = {{0}};

ifstream loadFile;
ofstream saveFile;

void draw_map();
bool map_walkable(int x, int y);
void load_file();
void save_file();

bool map_walkable(int x, int y)
{
    if (x>=0 && x <MAPW && y>=0 && y<MAPH )
    {
        if (myMap[y][x]==0) return true;
    }

    return false;
}

int Distance(int px, int py, int goalX, int goalY)
{
    return abs(px - goalX) + abs(py - goalY);
}
/*
bool FindPath (int startX, int startY, int goalX, int goalY, vector<Node> &path)
{

    path.clear();

    bool searching = true;
    bool doOnce = true;  // run only once ! s'execute qu'une fois !
    Node start = new Node(startX,startY);
    Node goal = new Node(goalX,goalY);
    Node current;

    vector <Node*> openList;
    vector <Node*> closedList;

    unsigned int n = 0;

    current = start;
    openList.push_back(start); // Add the start point in openList

    while (searching)
    {
        if (doOnce)
        {
            // 1 --- Check if goal is the same as starting point !
            if (goalX == startX && goalY == startY) return false;

            // 2 --- Check if target is walkable !
            if (!map_walkable(goalX,goalY)) return false;
            doOnce = true;
        }

        // 3 --- Put current Node in Closed list, Remove current of Open list

        if (current.x == startX && current.y == startY ) // first node to explore is start Node
        {
            closedList.push_back(current);
        }
        else
        {
            // search the lowest F !

        }



    }

    return false;
}
*/

int main(void)
{
    allegro_init();
    set_window_title ("MugenSoft");
    //install_timer();
    install_keyboard();
    install_mouse();

    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCRX, SCRY, 0, 0);

    buffer = create_bitmap(SCRX,SCRY);

    loadFile.open("myMap.mmf", ios::in);
    for (int j(0); j<MAPH; j++)
    {
        for (int i(0); i<MAPW; i++)
        {
            loadFile >> myMap[j][i];
            cout << myMap[j][i];
        }
        cout << "\n";
    }
    loadFile.close();

    bool key_f5;
    bool key_f1;
    bool key_del;

    depx = 2;
    depy = 2;

    arrx = 18;
    arry = 13;

    list <Node> openNode;
    Node current(depx,depy);
    list <Node>::iterator it;

    while (!key[KEY_ESC])
    {

        int selx = mouse_x/cs;
        int sely = mouse_y/cs;

        int posx = selx*cs;
        int posy = sely*cs;

        if (selx>-1 && sely>-1 && selx<MAPW && sely<MAPH)
        {
            if (mouse_b & 1)
            {
                myMap[sely][selx] = 1;
            }
            if (mouse_b & 2)
            {
                myMap[sely][selx] = 0;
            }
        }

        // Press Enter to Search Path !
        if (key[KEY_ENTER])
        {
            cout << "\n -- Search Mode On -- \n";
            cout << " Press [ <- ] to quit search !\n";

            //while (!key[KEY_BACKSPACE])
            //{
                unsigned int n(0);

                for (int x = -1; x < 2; x++)
                {
                    for (int y = -1; y < 2; y++)
                    {
                        if (x==0 && y==0) continue;

                        Node pNode(current.x,current.y);
                        pNode.x = current.x + x;
                        pNode.y = current.y + y;
                        if (x==0 || y==0) pNode.g = 10; else pNode.g = 14;
                        pNode.h = Distance(pNode.x, pNode.y, arrx, arry);
                        pNode.f = pNode.g + pNode.h;

                        openNode.push_back(pNode);

                        cout << n << "= "<< pNode.x << "," << pNode.y << endl ;
                        n++;
                    }
                }

                // récupération node avec le plus petit F !


            //}
            cout << "\n -- Search Mode Off -- \n";
        }

        // Press DEL to clear map !
        if (!key[KEY_DEL]) key_del = false;
        if (!key_del && key[KEY_DEL])
        {
            key_del=true;

            for (int j(0); j<MAPH; j++)
            {
                for (int i(0); i<MAPW; i++)
                {
                    myMap[j][i] = 0;
                    cout << myMap[j][i];
                }
                cout << "\n";
            }

        }

        // Press F5 to save map !
        if (!key[KEY_F5]) key_f5 = false;
        if (!key_f5 && key[KEY_F5])
        {
            key_f5=true;
            save_file();
        }

        // Press F1 to load map !
        if (!key[KEY_F1]) key_f1 = false;
        if (!key_f1 && key[KEY_F1])
        {
            key_f1=true;
            load_file();
        }

        clear_to_color(buffer, makecol(0,25,50));

        // Display OpenNode !

        for (it = openNode.begin(); it!=openNode.end(); ++it)
        {
            int px = (*it).x;
            int py = (*it).y;
            int f  = (*it).f;
            int g  = (*it).g;
            int h  = (*it).h;

            rectfill (buffer, px*cs,py*cs,px*cs+cs,py*cs+cs,makecol(0,200,10));
            textprintf_ex (buffer, font, px*cs+2, py*cs+2,makecol(255,200,0),-1,"(%i)",f);
            textprintf_ex (buffer, font, px*cs+2, py*cs+12,makecol(255,100,0),-1,"%i",g);
            textprintf_ex (buffer, font, px*cs+2, py*cs+22,makecol(255,0,100),-1,"%i",h);

        }

        draw_map();

        int c = cs/2;
        circlefill (buffer,depx*cs+c,depy*cs+c,8,makecol(0,150,50));
        circlefill (buffer,arrx*cs+c,arry*cs+c,8,makecol(150,10,0));


        rect(buffer,posx,posy,posx+cs,posy+cs,makecol(250,50,100));

        line(buffer, mouse_x, mouse_y-10, mouse_x, mouse_y+10, makecol(200,150,0));
        line(buffer, mouse_x-10, mouse_y, mouse_x+10, mouse_y, makecol(200,150,0));

        textprintf_ex (buffer, font, mouse_x+2, mouse_y+2,makecol(55,250,250),-1,"%d",Distance(posx/cs, posy/cs, arrx, arry));

        //int n(0);
        //textprintf_ex (buffer, font, 1, 1, -1, makecol(255,255,0), "-- Search Mode -- %i",n);

        blit(buffer, screen, 0, 0, 0, 0,SCRX,SCRY);
    }

    destroy_bitmap(buffer);
    allegro_exit();
    return 0;
}
END_OF_MAIN()

void draw_map()
{
    for (int j(0); j<MAPH; j++)
    {
        for (int i(0); i<MAPW; i++)
        {
            int cx = i*cs;
            int cy = j*cs;

            if (myMap[j][i]==1)
            {
                rectfill (buffer,cx,cy,cx+cs,cy+cs,makecol(0,150,200));
            }
            rect (buffer,cx,cy,cx+cs,cy+cs,makecol(0,50,100));
        }
    }

}

void load_file()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = win_get_window();
    ofn.lpstrFilter = "MugenMap Files (*.mmf)\0*.mmf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "mmf";

    if(GetOpenFileName(&ofn)) // linker -lcomdlg32
    {
        // Do something usefull with the filename stored in szFileName
        //loadFile.open("myMap.txt", ios::in);
        loadFile.open(szFileName, ios::in);

        for (int j(0); j<MAPH; j++)
        {
            for (int i(0); i<MAPW; i++)
            {
                loadFile >> myMap[j][i];
                cout << myMap[j][i];
            }
            cout << "\n";
        }
    }
    loadFile.close();
}
void save_file()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = win_get_window();
    ofn.lpstrFilter = "MugenMap Files (*.mmf)\0*.mmf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "mmf";

    if(GetSaveFileName(&ofn)) // linker -lcomdlg32
    {
        // Do something usefull with the filename stored in szFileName
        //loadFile.open("myMap.txt", ios::in);
        saveFile.open(szFileName, ios::out);
        for (int j(0); j<MAPH; j++)
        {
            for (int i(0); i<MAPW; i++)
            {
                saveFile << myMap[j][i];
                cout << myMap[j][i];
            }
            cout << "\n";
        }
    }
    saveFile.close();
}
