#include <ncurses.h>
#include <unistd.h>
#include <cwchar>
#include <locale>
#include <deque>
#include <stack>
#include <vector>

using namespace std;
#define WIDTH 61
#define HEIGHT 20

int score=0;
int speed=1;
int board[HEIGHT+1][WIDTH+1]={0};
int flag2=0;

void draw_board()
{
    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, WIDTH, ACS_URCORNER);
    mvaddch(HEIGHT, 0, ACS_LLCORNER);
    mvaddch(HEIGHT, WIDTH, ACS_LRCORNER);
    for(int i=1; i<WIDTH; i++)
    {
        mvaddch(0, i, ACS_HLINE);
        mvaddch(HEIGHT, i, ACS_HLINE);
    }
    for(int i=1; i<HEIGHT; i++)
    {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, WIDTH, ACS_VLINE);
    }
    mvprintw(HEIGHT, 1, "Score:%d ", score);
    refresh();
}
class snake
{
public:
    snake()
    {
        for(int i=1; i<6; i+=2)
        {
            body.push_back(make_pair(1, i));
        }
        _update_food();
    }

    void init()
    {
        body.pop_back();
        for(auto v:body)
        {
            _notshow_yx(v.first, v.second);
        }
        score=0;
        mvprintw(HEIGHT, 1, "Score:%d ", score);
        direct=2;
        //speed=15;
        body.clear();
        for(int i=1; i<6; i+=2)
        {
            body.push_back(make_pair(1, i));
        }
        draw_board();
        show();
    }

    void _show_yx(int y, int x)
    {
        attron(COLOR_PAIR(2));
        mvprintw(
        y, x,  
        "%lc%lc", (wint_t)0x2588, (wint_t)0x2588);
        attroff(COLOR_PAIR(2));
    }

    void _notshow_yx(int y, int x){ mvprintw(y, x, "  "); }

    void _update_food()
    {
        int flag=1;
        while(flag)
        {
            flag=0;
            food_pos=make_pair(rand()%(HEIGHT-1)+1,2*(rand()%(int)((WIDTH-1)/2))+1);
            for(auto v:body)
            {
                if(v==food_pos)
                {
                    flag=1;
                    break;
                }
            }
        }
        attron(COLOR_PAIR(1));
        mvprintw(
        food_pos.first, food_pos.second,  
        "%lc%lc", (wint_t)0x2588, (wint_t)0x2588);
        attroff(COLOR_PAIR(1));
        refresh();
    }


    void show()
    {
        for(auto v:body)
        {
            _show_yx(v.first, v.second);
        }
        attron(COLOR_PAIR(3));
        mvprintw(
        body.back().first, body.back().second,  
        "%lc%lc", (wint_t)0x2588, (wint_t)0x2588);
        attroff(COLOR_PAIR(3));
        refresh();
    }

    void update_show()
    {
        _notshow_yx(body.front().first, body.front().second);
        body.pop_front();
        show();
    }

    void update()
    {
        body.push_back(make_pair(
        body.back().first+direction[direct][0],
        body.back().second+direction[direct][1]));   
    }

    bool check_hit()
    {
        if(body.back().first<=0||body.back().second<=0||
           body.back().first>=HEIGHT||body.back().second>=WIDTH)
            return true;
        for(auto it=body.begin(); it!=body.end()-2; it++)
        {
            if(body.back()==*it)
                return true;
        }
        return false;
    }


    void eat_food()
    {
        if(body.back()==food_pos)
        {
            body.push_front(make_pair(HEIGHT+1, WIDTH+1));
            _show_yx(food_pos.first, food_pos.second);
            _update_food();
            score++;
            // if(!(score%5))
            //     speed/=2;
            mvprintw(HEIGHT, 1, "Score:%d ", score);
            refresh();
        }
    }

    void _init_board()
    {
        for(int i=0; i<HEIGHT+1; i++)
        {
            for(int j=0; j<WIDTH+1; j++)
            {
                if(board[i][j]!=6)
                    board[i][j]=-1;
                if(i==0||j==0||i==HEIGHT||j==WIDTH)
                    board[i][j]=5;
            }
        }
    }

    void ai()
    {
        deque<pair<int, int>> b;
        
        int y=body.back().first, x=body.back().second;
        for(int i=0; i<HEIGHT+1; i++)
        {
            for(int j=0; j<WIDTH+1; j++)
            {
                board[i][j]=0;
            }
        }

        _init_board();
        for(auto v:body)
        {
            board[v.first][v.second]=5;
        }
        for(int i=0; i<4; i++)
        {
            if(board[y+direction[i][0]][x+direction[i][1]]<0)
            {
                board[y+direction[i][0]][x+direction[i][1]]=i;
                b.push_back(make_pair(y+direction[i][0],x+direction[i][1]));
            }
        }
        int find=0;
        int find2=0;
        while(b.size())
        {
            y=b.front().first;
            x=b.front().second;
            b.pop_front();
            if(make_pair(y, x)==food_pos)
            {
                find=1;
                break;
            }
            for(int i=0; i<4; i++)
            {
                if(board[y+direction[i][0]][x+direction[i][1]]<0)
                {
                    board[y+direction[i][0]][x+direction[i][1]]=i;
                    b.push_back(make_pair(y+direction[i][0],x+direction[i][1]));
                }
            }
        }
        
        int d1;
        if(find)
        {
            pair<int, int>tail;
            b.clear();
            int k=body.size()-1;
            while(make_pair(y, x)!=body.back())
            {
                int m=y-direction[board[y][x]][0];
                int n=x-direction[board[y][x]][1];
                if(make_pair(m, n)==body.back())
                    d1=board[y][x];
                if(k>0)
                    board[y][x]=6;
                else if(k==0)
                {
                    tail=make_pair(y, x);
                }
                
                y=m;
                x=n;
                k--;
            }
            if(k>=0)
            {
                auto it=body.end();
                tail=*(it-k-1);
                for(it=it-k; it!=body.end(); it++)
                {
                    board[(*it).first][(*it).second]=6;
                }
            }
            
            _init_board();
            
            int m=food_pos.first;
            int n=food_pos.second;
            for(int i=0; i<4; i++)
            {
                if(board[m+direction[i][0]][n+direction[i][1]]<0)
                {
                    board[m+direction[i][0]][n+direction[i][1]]=i;
                    b.push_back(make_pair(m+direction[i][0],n+direction[i][1]));
                }
            }
            while(b.size())
            {
                m=b.front().first;
                n=b.front().second;
                b.pop_front();
                if(make_pair(m, n)==tail)
                {
                    find2=1;
                    break;
                }
                for(int i=0; i<4; i++)
                {
                    if(board[m+direction[i][0]][n+direction[i][1]]<0)
                    {
                        board[m+direction[i][0]][n+direction[i][1]]=i;
                        b.push_back(make_pair(m+direction[i][0],n+direction[i][1]));
                    }
                }
            }
            
            if(find2)
            {
                direct=d1;
                return;
            }
        }
        //find tail
        for(int i=0; i<HEIGHT+1; i++)
        {
            for(int j=0; j<WIDTH+1; j++)
            {
                board[i][j]=0;
            }
        }
        _init_board();
        y=body.back().first;
        x=body.back().second;
        stack<pair<int, int>> s;
        find=0;
        for(auto v:body)
        {
            board[v.first][v.second]=5;
        }
        int flag=1;
        if(body.front().first==HEIGHT+1&&body.front().second==WIDTH+1)
        {
            _notshow_yx(HEIGHT+1, WIDTH+1);
            body.pop_front();
            flag2++;
        }
        
        board[body.front().first][body.front().second]=-1;
        vector<int> t;
        for(int i=0; i<4; i++)
        {
            int j=rand()%4;
            int flag=1;
            while(flag)
            {
                flag=0;
                for(auto v:t)
                {
                    if(v==j)
                    {
                        j=rand()%4;
                        flag=1;
                        break;
                    }
                }
            }
            t.push_back(j);
            if(board[y+direction[j][0]][x+direction[j][1]]<0)
            {
                board[y+direction[j][0]][x+direction[j][1]]=j;
                s.push(make_pair(y+direction[j][0],x+direction[j][1]));
            }
        }

        while(s.size())
        {
            y=s.top().first;
            x=s.top().second;
            s.pop();
            if(make_pair(y, x)==body.front())
            {
                find=1;
                break;
            }
            t.clear();
            for(int i=0; i<4; i++)
            {
                int j=rand()%4;
                int flag=1;
                while(flag)
                {
                    flag=0;
                    for(auto v:t)
                    {
                        if(v==j)
                        {
                            j=rand()%4;
                            flag=1;
                            break;
                        }
                    }
                }
                t.push_back(j);
                if(board[y+direction[j][0]][x+direction[j][1]]<0||make_pair(y+direction[j][0],x+direction[j][1])==body.front())
                {
                    board[y+direction[j][0]][x+direction[j][1]]=j;
                    s.push(make_pair(y+direction[j][0],x+direction[j][1]));
                }
            }
        }
        if(find)
        {
            while(make_pair(y, x)!=body.back())
            {
                int m=y-direction[board[y][x]][0];
                int n=x-direction[board[y][x]][1];
                if(make_pair(m, n)==body.back())
                    d1=board[y][x];
                y=m;
                x=n;
            }
            direct=d1;
            if(flag2&&make_pair(body.back().first+direction[d1][0],body.back().second+direction[d1][1])!=body.front())
            {
                body.push_front(make_pair(HEIGHT+1, WIDTH+1));
                flag2--;
            }
            mvprintw(0,0,"%d",flag2);
            refresh();
        }
        else
        {
            mvprintw(0,0,"1");
            refresh();
            direct=rand()%4;
        }
        
    }

public:
    int direct=2;
private:
    deque<pair<int, int>> body;
    pair<int, int> food_pos;
    int direction[4][2]={{1, 0}, {-1, 0}, {0, 2}, {0, -2}};
};

int main(void)
{
    setlocale(LC_ALL, "");
    srand(time(0));
    initscr();

    noecho();
    curs_set(0);
    timeout(0);
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    draw_board();

    snake s;
    int tick=0;
    s.show();
    while(true)
    {
        if(tick>speed)
        {
            s.ai();
            int c=getch();
            if(c=='a')
            {
                if(s.direct!=2)
                    s.direct=3;
            }
            else if(c=='w')
            {
                if(s.direct!=0)
                    s.direct=1;
            }
            else if(c=='s')
            {
                if(s.direct!=1)
                    s.direct=0;
            }
            else if(c=='d')
            {
                if(s.direct!=3)
                    s.direct=2;
            }
            tick=0;
            s.update();
            s.update_show();
            if(s.check_hit())  
            {
                do
                {
                    c=getch();
                }while(c!='q'&&c!='r');
                if(c=='r')
                {   
                    s.init();
                }
                else
                    break;
            }
            s.eat_food();          
        }

        ++tick;
        usleep(100);
    }

    endwin();
}