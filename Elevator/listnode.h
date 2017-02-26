

struct listNode
{
	int f;//楼层
	char d;//类型
	int occupied;//被安排的电梯
	int running;//是否被执行
	int available;//wheather is being deleted
	struct listNode*nextPtr, *previousPtr;
};

typedef struct listNode LISTNODE;
typedef LISTNODE* LISTNODEPTR;
void add(LISTNODEPTR headPtr, LISTNODEPTR aPtr);
void del(LISTNODEPTR* aPtr);
LISTNODEPTR query(LISTNODEPTR headPtr, LISTNODEPTR aPtr, char direction);
LISTNODEPTR query(LISTNODEPTR headPtr, int f, char d);
LISTNODEPTR query(LISTNODEPTR headPtr, int f, char d, int para);
LISTNODEPTR next(LISTNODEPTR headPtr, LISTNODEPTR aPtr);
void freelist(LISTNODEPTR headPtr);
extern LISTNODE* NewNode(void);
extern LISTNODEPTR exsist(LISTNODE *headPtr, int f, char d);
extern void move(LISTNODEPTR headPtr, LISTNODEPTR aPtr);
extern int calc(LISTNODEPTR headPtr, int f1, int f2);

