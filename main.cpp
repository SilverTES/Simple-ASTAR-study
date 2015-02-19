#define ALLEGRO_STATICLINK

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <math.h>

#include <list>
#include <vector>
#include <algorithm>

#include <allegro.h>
#include <winalleg.h>

using namespace std;

const int SCRX(800);
const int SCRY(600);

const int MAPW(40);
const int MAPH(30);
const int cs = 20; // Taille des cases

struct Node
{
    int x;
    int y;
    int g;
    int h;
    int f;
    int parentX;
    int parentY;
    Node * parent;
};

struct f_comp
{
    bool operator()(const Node a, const Node b) const
    {
        return a.f < b.f;
    }
};

struct Vec2D
{
    int x;
    int y;
};

Vec2D adj[8];

int depx(0);
int depy(0);
int arrx(0);
int arry(0);

bool key_f5;
bool key_f1;
bool key_del;

bool quit = false;
bool goal = false;
bool noSolution = false;
bool key_enter = false;

Node current;
Node sucNode;

BITMAP * buffer(NULL);

char myMap[MAPH][MAPW] = {{0}};

ifstream loadFile;
ofstream saveFile;

vector <Node> openNode;
vector <Node> closedNode;
vector <Node> goalPath;
vector <Node>::iterator it;

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
    return 100*(fabs(px - goalX) + fabs(py - goalY));
}

bool isGoal (int x, int y)
{
    if (x==arrx && y==arry)
        return true;
    else
        return false;

}

bool isClosedNode(int sucX, int sucY)
{
    for (int i(0); i < closedNode.size(); i++)
    {
        //cout << "sucX = " << sucX << " = "<< closedNode[i].x << " " << "sucY = " << sucY << " = "<< closedNode[i].y << endl;
        if (sucX == closedNode[i].x && sucY == closedNode[i].y)
        {
            return true;
        }
    }
    return false;
}

bool isOpenNode(int sucX, int sucY)
{
    for (int i(0); i < openNode.size(); i++)
    {
        if (sucX == openNode[i].x && sucY ==openNode[i].y)
        {
            if (sucNode.f>openNode[i].f)
            {
                return true;
            }
            else
            {
                //openNode.erase(openNode.begin()+i);
                //openNode[i].parent = &current;
                return true;
            }
        }

    }
    return false;
}

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

    adj[0] = { 0,-1}; // U
    adj[1] = { 0, 1}; // D
    adj[2] = {-1, 0}; // L
    adj[3] = { 1, 0}; // R

    adj[4] = {-1,-1}; // UL
    adj[5] = {-1, 1}; // DL
    adj[6] = { 1,-1}; // UR
    adj[7] = { 1, 1}; // DR

    depx = 2;
    depy = 2;

    arrx = 37;
    arry = 27;

    current.x = depx;
    current.y = depy;
    current.parentX = -1;
    current.parentY = -1;
    current.parent = NULL;

    current.g = 0;
    current.h = Distance(depx,depy,arrx,arry);
    current.f = current.g + current.h;

    openNode.clear();
    closedNode.clear();

    openNode.push_back(current);

    while (!quit)
    {

        int selx = mouse_x/cs;
        int sely = mouse_y/cs;

        int posx = selx*cs;
        int posy = sely*cs;


        if (mouse_b & 1)
        {
            myMap[sely][selx] = 1;

        }
        if (mouse_b & 2)
        {
            myMap[sely][selx] = 0;
        }

        if (key[KEY_ESC]) quit = true;

        // Algo Pathfinding en action !


        if (!key[KEY_ENTER]) key_enter = false;
        if (key[KEY_RCONTROL]) key_enter = false;

        if (!goal && key[KEY_ENTER] && !key_enter && !noSolution)
        {
            key_enter = true;
            // Si openList vide -> Pas de solution !
            if (openNode.empty())
            {
                cout << " No solution " << endl ;
                noSolution = true;
            }


            // Trier openNode !
            if (openNode.size()>1) sort(openNode.begin(),openNode.end(), f_comp());

            // cherche le F le plus faible !

            //current = *min_element(openNode.begin(),openNode.end(),f_comp());

            //vector<Node*>::iterator result = min_element(openNode.begin(),openNode.end(),f_comp());

            //int currentpos = distance(openNode.begin(),result);

            current = openNode[0];

            // supprimer current de Open
            //openNode.erase(openNode.begin()+currentpos);
            openNode.erase(openNode.begin());
            // ajout current dans Closed
            closedNode.push_back(current);

            // Ajout des successor ! 8 x cases adjacent a tester !
            for (int i(0); i < 8; i++)
            {

                int x = (adj[i].x);
                int y = (adj[i].y);

                int sucX(current.x+x);
                int sucY(current.y+y);

                sucNode.x = sucX;
                sucNode.y = sucY;
                // Si succesor est dans CLosed , si oui on passe a la case suivante!
                if (isClosedNode(sucX,sucY)) continue;

                sucNode.parentX = current.x;
                sucNode.parentY = current.y;
                sucNode.parent = &current;

                // Si successor sort de la map on passe a la case suivante !
                if (sucX<0 || sucX>MAPW || sucY<0 || sucY>MAPH) continue;

                // Si succesor est dans obstacle on passe !
                if (!map_walkable(sucX,sucY)) continue;

                // Goal !! Si but atteint alors on quit la boucle !
                if (sucX==arrx && sucY==arry) { goal = true;}


                if (x==0 || y==0) sucNode.g = 10; else sucNode.g = 14; // calcul du coup du chemin parent -> successor , 1O tour droit , 14 en diagonale !

                sucNode.g = current.g+sucNode.g;
                sucNode.h = Distance(sucX, sucY, arrx, arry);

                sucNode.f = sucNode.g+sucNode.h;

                // Test si node successor a un meilleur F que le node dans Open !
                if (isOpenNode(sucX,sucY)) continue;

                openNode.push_back(sucNode);

            }


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

            rectfill (buffer, px*cs,py*cs,px*cs+cs,py*cs+cs,makecol(0,100,50));
            //textprintf_ex (buffer, font, px*cs+2, py*cs+2,makecol(255,200,200),-1,"%i",f);
            //textprintf_ex (buffer, font, px*cs+2, py*cs+12,makecol(120,200,0),-1,"%i",g);
            //textprintf_ex (buffer, font, px*cs+2, py*cs+22,makecol(0,120,200),-1,"%i",h);

        }
        // Display ClosedNode !
        for (int i(1); i < closedNode.size(); i++)
        {
            int os(cs/2);
            int x1(closedNode[i].x*cs+os);
            int y1(closedNode[i].y*cs+os);
            int x2(closedNode[i].parentX*cs+os);
            int y2(closedNode[i].parentY*cs+os);

            line(buffer,x1,y1,x2,y2,makecol(250,150,0));
        }

        for (it = closedNode.begin(); it!=closedNode.end(); ++it)
        {
            int px = (*it).x;
            int py = (*it).y;
            int f  = (*it).f;
            int g  = (*it).g;
            int h  = (*it).h;

            rectfill (buffer, px*cs+8,py*cs+8,px*cs+cs-8,py*cs+cs-8,makecol(200,200,0));
            //textprintf_ex (buffer, font, px*cs+2, py*cs+2,makecol(255,200,200),-1,"%i",f);
            //textprintf_ex (buffer, font, px*cs+2, py*cs+12,makecol(120,200,0),-1,"%i",g);
            //textprintf_ex (buffer, font, px*cs+2, py*cs+22,makecol(0,120,200),-1,"%i",h);
        }

        // Display Final Path !
        if (goal)
        {


        }



        rect (buffer, current.x*cs+1,current.y*cs+1,current.x*cs+cs-1,current.y*cs+cs-1,makecol(0,150,250));

        draw_map();

/*
        unsigned int n(0);
        for (it = closedNode.begin(); it!=closedNode.end(); ++it)
        {
            textprintf_ex (buffer, font, 2, n*10+2,makecol(250,120,250),-1,"%i,%i=%i",(*it).x,(*it).y,(*it).f);
            n++;
        }

        n = 0;
        for (it = openNode.begin(); it!=openNode.end(); ++it)
        {
            textprintf_ex (buffer, font, 720, n*10+2,makecol(200,120,0),-1,"%i,%i=%i",(*it).x,(*it).y,(*it).f);
            n++;
        }
*/

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
                rectfill (buffer,cx,cy,cx+cs,cy+cs,makecol(0,100,120));
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
