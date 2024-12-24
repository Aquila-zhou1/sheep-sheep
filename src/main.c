/*
 * 问题一：level 1有不字符卡排(已解决）
 * 问题二：level 1无解（已解决）
 * 问题三：level 3及以上的层数和设想有出入（level 3甚至只有一层，且无解）(已解决）
 * 顺便改了点好看的颜色
 * 问题四：level 7之上无法正确处理最上层(已解决）
 * 问题五：失败时显示不出来(已解决)
 * 问题六：附加卡槽没法选择
 */
//    cd C:\Users\A\CLionProjects\untitled2\cmake-build-debug
//   .\untitled2.exe
#include <conio.h>  // 必须
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>   // 必须

#define  Y0   0
#define  LEN 10 //map len seen
#define  TYPE 20
#define  NUM 40 //max number of each type of card
#define  FLOOR 20
#define  UP 72
#define  DOWN 80
#define  LEFT 75
#define  RIGHT 77
int len_capacity[LEN+1];
int card[TYPE+1]={0};
char position[FLOOR+1][LEN+1][LEN+1];
int transform[FLOOR+1][3]={0};   //第【1】列表示r第【2】列表示c
char map[FLOOR+1][3*LEN+4+8][5*LEN+6];
char standard[4][6];
int map_red[2]={0};//对应行列
int historical_site[4]={0};//A 行 列 高
char hand[7]={0};
char hand2[10]={0};
HANDLE consoleHandle;  // 操作控制台（也就是那个黑框框）需要的一个变量
int w, h;              // 高度，宽度，对应 y 和 x



// 在屏幕上 y, x 的位置打印一个 ch
void mvaddch(int y, int x, char ch) {
    COORD co = (COORD){.X = x, .Y = y};
    SetConsoleCursorPosition(consoleHandle, co);  // 设置你的光标位置
    putchar(ch);                                  // 在这里打印一个字符
}
// 类似地，你可以以这种方式定义一些在某个位置打印字符串的函数
void mvaddstr(int y, int x, char *ch) {
    COORD co = (COORD){.X = x, .Y = y};
    SetConsoleCursorPosition(consoleHandle, co);  // 设置你的光标位置
    for(int i=0; i<strlen(ch); i++)
    {
        putchar(*(ch+i));                                  // 在这里打印一个字符
        Sleep(10);               // 程序暂停 10 毫秒
    }
}



//生成从1到max，n个不同的随机数并存储到s中
void random(int *s,int max,int n)
{
    srand((unsigned int)time(NULL));//随机数种子
    for(int i=0; i<n; i++)
    {
        int bl=1;
        while(bl)//只要有一个随机数与前几个相等就重来
        {
            bl=0;
            *(s+i) = rand()%(max)+1;//从1到max的随机数
            for(int j=0; j<i; j++)
            {
                if(*(s+j)==*(s+i))
                {
                    bl++;//只要有一个数相等就循环，直到符合要求了再来下一步
                    break;
                }
            }
        }
    }
}

//修改position的某一层，返回*card_turn；sum为剩余牌的总数
char *fill_floor(int floor, int len, char *card_turn, int sum)
{
    int *p=(int *) malloc(len_capacity[LEN] *sizeof(int ));
    int n;//这一层要用的card数量
    if(sum<len_capacity[len])       //卡牌数小于这一层的容量
        n=sum;
    else n=len_capacity[len];         //卡牌足够这一层
    random(p, len*len, n);     //随机n个位置
    int r,c;
    for(int i=0; i<n; i++)
    {
        r=(*(p+i)-1)/len+1;
        c=(*(p+i)-1)%len+1;
        position[floor][r][c]=*(card_turn+i);     //发牌
        //mvaddch(13+floor,i,position[floor][r][c]);///////////////////////////debug
    }
    //mvaddstr(floor+10,0,"I have filled one floor! It's len is ");        //debug
    //printf("%d",len);
    return card_turn+n;              //返回剩下的卡牌
}

//生成地图
void create_position(int level, int type)
{
    //统计有多少个卡片并洗牌
    int sum=0;
    for(int i=1; i<=type; i++)
    {
        sum+=card[i];          //现有卡牌总数
    }
    char *card_turn = (char *) malloc(TYPE*NUM *sizeof(char));
    int *s=(int *) malloc(TYPE*NUM *sizeof(int));
    random(s, sum, sum);           //将1到sum打乱存入*s
    for(int i=0; i<sum; i++)      //*s一个一个过
    {
        for(int j=1; j<=type; j++)//遍历card[]的种类
        {
            if(*(s+i)<=card[j])            //找到这个数表示哪种牌
            {
                *(card_turn+i)='A'+j-1;    //实现随机数与卡牌的转换
                break;
            }
            else *(s+i)-=card[j];            //没找到继续找
        }
    }
    //mvaddstr(19,0,"card_turn:");
    //mvaddstr(20,0,card_turn);    //、、、、、、、、、、、、、、debug

    //create position 并初始化
    int t;
    int floor;
    int len;
    if(level<=2)
    {
        len=2;
        floor=level;
    }
    else
    {
        len=level;
        t=len-2;
        floor=2*t;
    }

    for(int i=1; i<=floor; i++)      //position 的初始化
    {
        for(int j=1; j<=len; j++)
        {
            for(int k=1; k<=len; k++)
                position[i][j][k]=' ';
        }
    }
    if(level<=2)    //两级以下的position
    {
        for(int i=1; i<=floor; i++)
        {
            card_turn= fill_floor(i, len, card_turn, sum);
            if(sum<=len_capacity[2])     //fill_floor函数不会改变sum的值
                break;
            else sum-=len_capacity[2];
        }
    }
    else            //level>=3
    {
        //生成底部position
        for(int i=1; i<=t; i++)
        {
            card_turn= fill_floor(i,3,card_turn, sum);
            if(sum<=len_capacity[3])
            {
                sum=0;
                break;
            }
            else sum-=len_capacity[3];
        }
        //生成扩展position
        for(int i=1; sum; i++)
        {
            int l=i+2;       //从len=3开始
            card_turn= fill_floor(t+i,l,card_turn,sum);    //发牌填层
            if(sum<=len_capacity[l])
            {
                sum=0;
            }
            else sum-=len_capacity[l];
        }
    }
/*
    for(int i=1; i<=len; i++)   //行
    {
        for(int j=1; j<=len; j++)   //列
            mvaddch(i+20, j, position[1][i][j]);  //debug
    }
    */
}

void  create_transform(int level, int floor)
{
    int *p=(int *) malloc(FLOOR *sizeof(int ));
    random(p, FLOOR, floor);     //随机n个位置
    for(int i=1; i<=floor; i++)//行
    {
        transform[i][1]=*(p+i-1)%3+1;
        transform[i][2]=*(p+i-1)%5+1;
        //mvaddch(22+i,0,'0'+transform[i][1]);///////////////////////////debug
        //mvaddch(22+i,1,'0'+transform[i][2]);///////////////////////////debug
    }
}


//由position转化为map
void  position_map(int level)
{
    int t;
    int floor;
    int len;
    if(level<=2)
    {
        len=2;
        floor=level;
    }
    else
    {
        len=level;
        t=len-2;
        floor=2*t;
    }

    //map初始化
    for(int i=1; i<=floor; i++)//层
    {
        for(int j=1; j<=3*LEN+3; j++)//行
        {
            for(int k=1; k<=5*LEN+5; k++)//列
                map[i][j][k]=' ';
        }
    }

    for(int i=1; i<=floor; i++)//position 层数
    {
        int len_floor;
        if(level<=2)
            len_floor=2;
        else
        {
            if(i<=t)
                len_floor=3;
            else
                len_floor=(i-t)+2;  //从len=3开始
        }
        int r=transform[i][1]+(LEN-len_floor)*3/2;  //position行列与地图坐标的转换
        int c=transform[i][2]+(LEN-len_floor)*5/2;
        for(int j=1; j<=len; j++)//position 行
        {
            for(int k=1; k<=len; k++)//position 列
            {
                if(position[i][j][k]!=' ')
                {
                    int row=3*(j-1)+r-1;//地图行
                    int col=5*(k-1)+c-1;//地图列
                    for(int p=1; p<=3; p++)
                    {
                        for(int q=1; q<=5; q++)
                        {
                            map[i][row+p][col+q]=standard[p][q];//形成整个卡牌
                        }
                    }
                    map[i][row+2][col+3]=position[i][j][k];//卡牌中心
                    //mvaddstr(28+(i-1)*len*len+j*len+k,0,"I have print a card ");       //debug
                    //printf("%d%d%d%c",i,j,k,position[i][j][k]);
                }
            }
        }
    }
}

//由map倒推position，用于取走卡牌
void map_position(int level)
{
    int floor;
    for(int i=FLOOR; i>0; i--)
    {
        if(map[i][historical_site[1]][historical_site[2]] != ' ')
        {
            floor=i;
            break;
        }
    }


    int len_floor,t;
    if(level<=2)
        len_floor=2;
    else
    {
        t=level-2;
        if(floor<=t)
            len_floor=3;
        else
            len_floor=(floor-t)+2;  //从len=3开始
    }
    int r=transform[floor][1]+(LEN-len_floor)*3/2;  //position行列与地图坐标的转换
    int c=transform[floor][2]+(LEN-len_floor)*5/2;
    //                    int row=3*(j-1)+r-1;//地图行
    //                    int col=5*(k-1)+c-1;//地图列
    int row=(historical_site[1]-r-1)/3+1;
    int col=(historical_site[2]-c-2)/5+1;
    //                    map[i][row+2][col+3]=position[i][j][k];//卡牌中心
    position[floor][row][col]=' ';
}

//地图与屏幕坐标的转换
//行
int y_row(int row)
{
    int y=row+Y0;
    return y;
}
//列
int x_col(int col, int wide)
{
    int x=(wide-(5*LEN+4))/2;//为居中而预留的距离
    x+=col;
    return x;
}

//卡槽与map的转换
void hand_map()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//普通白字
    int edge=(5*LEN+5-7*5)/2;
    int row=3*LEN+3+6;
    int row2=3*LEN+3+2;
    int col=edge+3;//起始列
    int col2=3;
    //hand
    for(int i=0; i<7; i++)
    {
        for(int p=1; p<=3; p++)
        {
            for(int q=1; q<=5; q++)
            {
                if(hand[i])
                    map[FLOOR][row-2+p][col-3+q]=standard[p][q];//形成整个卡牌
                else
                    map[FLOOR][row-2+p][col-3+q]=' ';
            }
        }
        map[FLOOR][row][col]=hand[i];
        col+=5;
    }
    //hand2
    for(int i=0; i<10; i++)
    {
        for(int p=1; p<=3; p++)
        {
            for(int q=1; q<=5; q++)
            {
                if(hand2[i])
                    map[FLOOR][row2-2+p][col2-3+q]=standard[p][q];//形成整个卡牌
                else
                    map[FLOOR][row2-2+p][col2-3+q]=' ';
            }
        }
        map[FLOOR][row2][col2]=hand2[i];
        col2+=5;
    }

    //for(int i=10; i<=17; i++)
    //mvaddch(i,0,' ');


    /*mvaddch(10,0,' ');
    printf("My hand2 :\n");
    for(int i=0; i<10; i++)
    {
        mvaddstr(11+i,0,"  ");
        mvaddch(11+i,0,' ');
        printf("%c\n",hand2[i]);
    }////////////////debug*/
}


//检测是否为最上层的部分，return 1； 否则return0
int check_seen(int floor, int row, int col, int wide)//wide为屏幕信息
{
    if(map[floor][row][col]!=0)
    {
        int bl=0;//用这个作判断
        for(int i=FLOOR; i>floor; i--)//从上到下
        {
            if(map[i][row][col]!=' ')
            {
                bl++;//说明上面有遮蔽
                break;
            }
        }
        if(bl==0)//是最上层
        {
            return 1;
        }
    }
    return 0;//不是最上层
}

//检测是否为最上层完整卡牌，并打印出来，return 1；否则return 0
int check_whole(int floor, int row, int col, int wide)
{
    if(row>=3 && row<=3*LEN+3+6 && col>=3 && col<=5*LEN)//防止出界
    {
        for(int i=1; i<=3; i++)//行
        {
            for(int j=1; j<=5; j++)//列
            {
                int r=row+i-2;//行周围
                int c=col+j-3;//列周围
                //mvaddstr(13,0,"I have check whole");      //debug
                if(check_seen(floor, r, c, wide)==0  ||  ((i!=2 || j!=3) && map[floor][r][c]!=standard[i][j]))//只要有一个部分不是最上层或不符合卡牌样式
                    return 0;
            }
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 123);//白底亮绿字
        for(int i=-1; i<=1; i++)//行
        {
            for(int j=-2; j<=2; j++)//列
            {
                mvaddch(y_row(row)+i, x_col(col,wide)+j, map[floor][row+i][col+j]);//打印整个牌
            }
            //mvaddstr(14,0,"I get it, check whole*2!!!!");                                              //debug
        }
        return 1;
    }
    return 0;//出界了
}

//算差
int difference(int a, int b)
{
    int x=a-b;
    if(x<0)
        x=-x;
    return x;
}

//打包函数
int pack(const int wide, int r, int c)
{
    for(int k=FLOOR; k>=1; k--)
    {
        if(map[k][r][c]!=' ' && check_seen(k,r,c,wide) && check_whole(k,r,c,wide))
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);//白底红字
            for(int i=-1; i<=1; i++)//行
            {
                for(int j=-2; j<=2; j++)//列
                {
                    mvaddch(y_row(r)+i, x_col(c,wide)+j, map[k][r+i][c+j]);//打印整个牌
                }
            }
            //mvaddstr(0,0,"success");///////////////////////////////////debug
            map_red[0]=r;
            map_red[1]=c;
            //mvaddstr(20,0,"the floor is ");///////////////////////debug
            //printf("%d",k);////////////////////////////////
            return 1;
        }
    }
    return 0;
}
//把地图打印出来，把最上层card标出来,并把距离【y】【x】最近的卡牌标红
void show_map(const int level,const int wide, int y, int x)
{
    int edge=(5*LEN+5-7*5)/2;
    //重来 清卡槽
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    for(int i=1; i<=3*LEN+3+7; i++)//行
    {
        for(int j= 1; j<=5*LEN+5; j++)//列
            mvaddch(y_row(i) , x_col(j,wide) ,' ');
    }

    int distance=100000;//初始化距离
    int r_next=0;//初始化地图上的位置
    int c_next=0;
    int h_next=0;
    //int debug=0;
    for(int i=1; i<=3*LEN+3+8; i++)//行
    {
        for(int j=1; j<=5*LEN+4; j++)//列
        {
            for(int k=FLOOR; k>=1; k--)//从上到下 层数
            {
                if(map[k][i][j]!=' ')
                {
                    //debug++;
                    //mvaddstr(100,0,"  ");         ////////////debug
                    //printf("primary success  :%d %d %d %d\n",k,i,j,debug);
                    if(check_seen(k,i,j,wide))//最上层
                    {
                        //printf("it is seen");                 //debug
                        if(check_whole(k,i,j,wide))//整个牌在最上层
                        {
                            int temp= difference(y_row(i),y) + difference(x_col(j,wide),x);//牌与该点的距离
                            //mvaddch(9,0,' ');
                            //printf("temp=%d",temp);                                        //debug
                            if(temp<distance)
                            {
                                distance=temp;//存档更近的距离
                                r_next=i;//存档最近的点
                                c_next=j;
                                h_next=k;
                            }
                            j+=2;           //跳过对同行下两列的检验（卡牌大小3*5）
                        }
                        else if(check_whole(k,i-1,j+2,wide) || check_whole(k,i+1,j+2,wide))  //是一个完整卡牌的左下角或右上角
                            j+=4;                      //跳过对同行下四列的检验
                        else                                    // 部分在最上层
                        {
                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 137);//灰底深蓝字
                            //mvaddstr(11,0,"I get it!");                                                                //debug
                            mvaddch(y_row(i), x_col(j,wide), map[k][i][j]);
                        }
                    }
                    break;//找到的第一个即为最上层，退出并查找其他点
                }
            }
        }
    }//标红最近的点
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);//白底红字
    //printf("I'm going to print red");              //debug
    map_red[0]=r_next;
    map_red[1]=c_next;
    y=y_row(r_next);//屏幕坐标
    x=x_col(c_next,wide);
    for(int i=-1; i<=1; i++)//行
    {
        for(int j=-2; j<=2; j++)//列
            mvaddch(y+i,x+j,map[h_next][r_next+i][c_next+j]);//打印整个牌
    }

    //打印地图边界
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//普通白字
    for(int i=0; i<=5*LEN+5; i++)      //i表示列
    {
        mvaddch(y_row(0), x_col(i,wide),'+');//打印上边框
        mvaddch(y_row(3*LEN+3), x_col(i,wide),'+');//打印下边框
        if(i>=edge && i<=5*LEN+5-edge)
        {
            mvaddch(y_row(3*LEN+3+4), x_col(i,wide),'+');//打印上卡槽
            mvaddch(y_row(3*LEN+3+8), x_col(i,wide),'+');//打印下卡槽
        }
    }
    for(int i=0; i<=3*LEN+3; i++)      //i表示行
    {
        mvaddch(y_row(i), x_col(0, wide), '+'); //打印左边框
        mvaddch(y_row(i), x_col(5 * LEN + 5, wide), '+'); //打印右边框
    }
    for(int i=3*LEN+3+4; i<=3*LEN+3+8; i++)   //i表示行
    {
        mvaddch(y_row(i), x_col(edge,wide),'+');//打印左卡槽
        mvaddch(y_row(i), x_col(5*LEN+6-edge,wide),'+');//打印右卡槽
    }
}


int main() {
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);   // 初始化这个操作器
    CONSOLE_SCREEN_BUFFER_INFO csbi;         //f        // 屏幕的信息
    GetConsoleScreenBufferInfo(consoleHandle, &csbi);  // 获取屏幕信息
    w = csbi.dwSize.X;
    h = csbi.dwSize.Y;  // 得到宽度高度
    // 游戏里面，如果一直有输入的光标，就有点不好看。我们可以让它不显示
    CONSOLE_CURSOR_INFO cci;  // 光标信息
    /*cci.dwSize = 100;
    cci.bVisible = FALSE;                       // 不可见
    SetConsoleCursorInfo(consoleHandlew, &cci);  */ //将光标特性应用到控制台
    // 到这里，闪烁的光标就消失了。

    /*for(int i=0; i<=w; i++)
    {
        for(int j=0; j<=h; j++)
            mvaddch(j,i, " ");
    }*/

    /*对standard牌样式的初始化
    ┌---┐
    |<X>|
    └---┘
     */
    //四角
    SetConsoleOutputCP(437);
    standard[1][1]=218;
    standard[1][5]=191;
    standard[3][1]=192;
    standard[3][5]=217;
    //四周
    standard[2][1]='|';
    standard[2][5]='|';
    for(int i=2; i<=4; i++)
    {
        standard[1][i]='-';
        standard[3][i]='-';
    }
    //里面
    standard[2][2]='<';
    standard[2][4]='>';


    //len_capacity的第i个元素表示边长为i的position的卡牌容量
    for(int i=1; i<=LEN; i++)
        len_capacity[i]=(i*i/5+1)*3;

    //清屏
    mvaddch(0,0,' ');
    for(int i=0; i<=6*w; i++)
        printf(" ");

    mvaddstr(0,0,"Welcome! ");
    Sleep(500);               // 程序暂停 500 毫秒
    mvaddstr(3,0,"Press to continue:");
    while (1)
    {
        //游戏刚开始或重开
        while(1)
        {
            mvaddstr(4,0,"Now choose a level(from 1 to 10 ):");
            int level;            //等级
            scanf("%d",&level);
            int f;            //一个关于层数的很重要的量
            int floor;        //总层数
            int len;          //position最大边长
            int type;         //卡牌种类
            if(level<=2)
            {
                len=2;
                floor=level;
                type=level;
                for(int i=1; i<=type; i++)
                    card[i]=3;      //每种3个
            }
            else
            {
                len=level;
                f=len-2;
                floor=2*f;
                type=2*level;
                int sum=f*len_capacity[3];      //底层卡牌数
                for(int i=3; i<=len; i++)
                {
                    sum+=len_capacity[i];      //上层卡牌数
                }
                sum=sum/3;                    //保证为3的倍数
                for(int i=1; i<=type; i++)
                    card[i]=sum/type*3;         //卡牌均匀分配
                for(int i=1; i<=sum%type; i++)
                    card[i]+=3;            //剩余卡牌均匀分配
            }
            //mvaddch(18,0,' ');
            //for(int i=1; i<=type; i++)
            //printf("%d",card[i]);                  //debug
            mvaddstr(5,0,"LET'S GO!");

            //对map的初始化

            for(int i=1; i<=3*LEN+3+8; i++)//行
            {
                for (int j = 1; j <= 5 * LEN + 5; j++)//列
                {
                    for (int k = FLOOR; k >= 1; k--)//从上到下 层数
                        map[k][i][j] = ' ';
                }
            }



            //生成主地图
            create_position(level,type);
            create_transform(level, floor);
            position_map(level);
            show_map(level,w,1,1);
//第二部分：游戏中
            mvaddch(0,w-1,' ');

            while(1)
            {
                while (kbhit() == 0)
                {
                    if(kbhit()!=0)
                        break;
                }
                char t=getch();
                // 上下左右w s a d
                if(t=='w' || t=='s' || t=='a' || t=='d' || t==UP || t== DOWN || t==LEFT || t==RIGHT)
                {
                    for(int k=FLOOR; k>=1; k--)//从上到下 层数
                    {
                        if(map[k][map_red[0]][map_red[1]] != ' ' && check_whole(k,map_red[0],map_red[1],w))
                        {
                            //mvaddstr(1,0,"sssssss"); ///////////////////debug
                            break;//白底蓝绿字
                        }
                    }
                    int bl=1;
                    if(t=='d' || t==RIGHT)//右
                    {
                        for(int i=map_red[0]; bl && i>0; i--)//行
                        {
                            for(int j=map_red[1]+1; bl && j<=5*LEN+6; j++)//列
                            {
                                if(pack(w,i,j))
                                    bl--;
                            }
                        }
                    }
                    else if(t=='a' || t==LEFT)//左
                    {
                        for(int i=map_red[0]; bl && i<=3*LEN+3+4; i++)//行
                        {
                            for(int j=map_red[1]-1; bl && j>0; j--)//列
                            {
                                if(pack(w,i,j))
                                    bl--;
                            }
                        }
                    }
                    else if(t=='w' || t==UP)//上
                    {
                        for(int i=map_red[1]; bl && i>0; i--)//列
                        {
                            for(int j=map_red[0]-1; bl && j>0; j--)//行
                            {
                                if(pack(w,j,i))
                                    bl--;
                            }
                        }
                    }
                    else //下
                    {
                        for(int i=map_red[1]; bl && i<=5*LEN+4; i++)//列
                        {
                            for(int j=map_red[0]+1; bl && j<=3*LEN+3+4; j++)//行
                            {
                                if(pack(w,j,i))
                                    bl--;
                            }
                        }
                    }
                    if(bl==1)
                        pack(w,map_red[0],map_red[1]);
                }
                    // 选取 空格
                else if(t==' ')
                {
                    int high;
                    for(int i=FLOOR; i>=0; i--)
                    {
                        if(map[i][map_red[0]][map_red[1]]!=' ')
                        {
                            high=i;
                            break;
                        }
                    }
                    historical_site[0]=map[high][map_red[0]][map_red[1]]-'A'+1;//卡牌种类 1代表A
                    historical_site[1]=map_red[0];//行
                    historical_site[2]=map_red[1];//列
                    historical_site[3]=high;
                    for(int i=0; i<7; i++)
                    {
                        if(hand[i]==0)//卡槽这一格为空
                        {
                            hand[i]='A'+historical_site[0]-1;
                            break;
                        }
                        else if(map[high][map_red[0]][map_red[1]]<=hand[i])//排序先于原有位置
                        {
                            for(int j=6; j>i; j--)//从后向前瞬移
                                hand[j]=hand[j-1];
                            hand[i]=map[high][map_red[0]][map_red[1]];
                            break;
                        }
                    }
                    for(int i=0; i<=5; i++)//看有没有三个能消掉的
                    {
                        if(hand[i]== hand[i+1]  &&  hand[i]==hand[i+2])
                        {
                            for(int j=i; j<=i+2; j++)//消消乐
                                hand[j]=0;
                            for(int j=i+3; j<7 && hand[j]; j++)//顺延
                            {
                                hand[j-3]=hand[j];
                                hand[j]=0;
                            }
                        }
                    }
                    if(historical_site[1]>=3*LEN+3)//选取了hand2
                    {
                        int col2=3;
                        int temp=(historical_site[2]-col2)/5;
                        for(int i=temp; i<9 && hand2[i+1]; i++)
                        {
                            hand2[i]=hand[i+1];
                            hand[i+1]=0;
                        }
                        /*mvaddstr(7,0,"sssssss7");
                        mvaddch(15,0,' ');
                        printf("My hand2 :\n");
                        for(int i=0; i<10; i++)
                        {
                            mvaddstr(16+i,0,"  ");
                            mvaddch(16+i,0,' ');
                            printf("%c",hand2[i]);
                        }////////////////debug*/
                    }
                    hand_map();//卡槽转换为map
                    //map回到position

                    if(historical_site[1]<3*LEN+3)//选取了不是hand2
                        map_position(level);
                    position_map(level);
                    show_map(level,w, y_row(historical_site[1]), x_col(historical_site[2],w));
                    if(hand[6])//卡槽满了
                    {
                        //重来 清屏
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        for(int i=Y0; i<=Y0+3*LEN+4+8; i++)
                        {
                            for(int j=0; j<=w; j++)
                                mvaddch(i,j,' ');
                        }
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);//白底红字
                        mvaddch(0,0,' ');
                        printf("Game over\nPress 'p' to start again");
                    }
                }
                    // 重新开始 p(play)
                else if(t=='p')
                {
                    for(int i=0; i<=3; i++)
                        historical_site[i]=0;
                    for(int i=0; i<7; i++)
                        hand[i]=0;
                    for(int i=0; i<10; i++)
                        hand2[i]=0;
                    break;
                }
                    // 从卡槽取出三张牌 j
                else if(t=='j')
                {
                    //消hand填hand2
                    for(int i=6; i>=0; i--)
                    {
                        if(hand[i])//从后向前数最后一个牌
                        {
                            for(int j=0; j<10; j++)//从前向后数第一个空位
                            {
                                if(hand2[j]==0)
                                {
                                    hand2[j]=hand[i];
                                    break;
                                }
                            }
                            hand[i]=0;
                            break;
                        }
                    }
                    hand_map();
                    show_map(level,w, y_row(historical_site[1]), x_col(historical_site[2],w));
                }
                if(hand2[9])//卡槽满了
                {
                    //重来 清屏
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    for(int i=Y0; i<=Y0+3*LEN+4+8; i++)
                    {
                        for(int j=0; j<=w; j++)
                            mvaddch(i,j,' ');
                    }
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);//白底红字
                    mvaddch(0,0,' ');
                    printf("Game over\nPress 'p' to start again");
                }
                    // 打乱 k
                else if(t=='k')
                {
                    //统计card
                    for(int i=1; i<=TYPE; i++)
                        card[i]=0;
                    for(int i=1; i<=floor; i++)//层
                    {
                        for(int j=1; j<=len; j++)//行
                        {
                            for(int k=1; k<=len; k++)//列
                            {
                                if(position[i][j][k] !=' ')
                                {
                                    int p=position[i][j][k]-'A'+1;
                                    card[p]++;
                                }
                            }
                        }
                    }

                    create_position(level,type);
                    create_transform(level, floor);
                    position_map(level);
                    show_map(level,w,1,1);
                }
                    // 撤回一张 l
                else if(t=='l')
                {
                    if(historical_site[0])
                    {
                        //消hand
                        char ch='A'+historical_site[0]-1;
                        int p;
                        for(int i=0; i<7; i++)
                        {
                            if(hand[i]==ch)
                            {
                                p=i;
                                break;
                            }
                        }
                        for(int i=p; i<6; i++)
                            hand[i]=hand[i+1];
                        hand_map();
                        //还原position
                        int len_floor,te;
                        if(level<=2)
                            len_floor=2;
                        else
                        {
                            te=level-2;
                            if(floor<=te)
                                len_floor=3;
                            else
                                len_floor=(historical_site[3]-te)+2;  //从len=3开始
                        }
                        int r=transform[floor][1]+(LEN-len_floor)*3/2;  //position行列与地图坐标的转换
                        int c=transform[floor][2]+(LEN-len_floor)*5/2;
                        //                    int row=3*(j-1)+r-1;//地图行
                        //                    int col=5*(k-1)+c-1;//地图列
                        int row=(historical_site[1]-r-1)/3+1;
                        int col=(historical_site[2]-c-2)/5+1;
                        //                    map[i][row+2][col+3]=position[i][j][k];//卡牌中心
                        position[historical_site[3]][row][col]=ch;
                        position_map(level);
                        show_map(level,w, y_row(historical_site[1]), x_col(historical_site[2],w));
                        for(int i=0; i<=3; i++)
                            historical_site[i]=0;
                    }
                }
                    // 疑问 q
                else if(t=='q')
                {}
                //else mvaddstr(0,0,"Improper input");
            }
            // 确认 y/n
            //卡牌的选择与选取
//结束
/*while (kbhit() != 0) {  // 如果它检测到有键盘敲击，返回非零。没有则返回 0
            /ar c = getch();// 有键盘敲击，我们获取是哪一个键

            }*/
            //重来 清屏
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            for(int i=Y0; i<=Y0+3*LEN+4+8; i++)
            {
                for(int j=0; j<=w; j++)
                    mvaddch(i,j,' ');
            }




            //ar a[2]="i,u";
            //mvaddch(h / 2, w / 2, c);
            /*
            mvaddch(h / 2+1, w / 2, a[1]);  // 显示在屏幕中心
            mvaddch(h / 2, w / 2+1,a );
            //Sleep(500);               // 程序暂停 1000 毫秒
            SetConsoleOutputCP(437);
            mvaddch(h / 2-1 ,w / 2-1,2);*/
        }
    }
    // 游戏结束，我们要恢复光标显示
    cci.bVisible = TRUE;                        // 可见
    SetConsoleCursorInfo(consoleHandle, &cci);  // 重新设置
    SetConsoleOutputCP(437);
    mvaddch(h / 2 ,w / 2,218);
    /*printf("%c%c\n%c%c",218,191,192,217);
    printf("The color is white now. U'll change it to red.\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    printf("The color is red now. U'll change it again\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("It turns back to white.");*/

}
