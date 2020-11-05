#include "table.h"

table::table() {
    m_Fd = open(RECORD_FILE, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (m_Fd == -1)
        throw "In table::table(),open error";

    //读取文件中已有的记录数量
    records = new Record[MAX_RECORD_NUM];
    //判断表格是否为空
    if (lseek(m_Fd, 0, SEEK_END) == 0) {
        record_num = 0;
        InitializeTable();
    } else {
        record_num = lseek(m_Fd, 0, SEEK_END) / RECORD_SIZE_BYTE;
        lseek(m_Fd, 0, SEEK_SET);
        read(m_Fd, records, record_num * RECORD_SIZE_BYTE);
    }
    tree = new BPlusTree;
    srand((unsigned) time(NULL));//随机数种子

    //初始化mutex
    m_pMutexForOperatingTable = new pthread_mutex_t;
    if (pthread_mutex_init(m_pMutexForOperatingTable, 0) != 0 ) {
        delete m_pMutexForOperatingTable;
        close(m_Fd);
        throw "In table::table(),pthread_mutex_init error";
    }
}

//析构函数
table::~table() {
    if (m_Fd) {
        close(m_Fd);
    }
    delete[] records;
}

void table::InitializeTable() {
    Record record;
    for (int i = 0; i < 10000; i++) {
        record = CreateRecord();
        if (!AppendRecord(record))
            throw "In table::table(),init error";
    }
}


pthread_mutex_t* table::m_pMutexForCreatingTable = table::InitializeMutex();
table* table::m_table = 0;

//初始化创建表格的互斥量
pthread_mutex_t *table::InitializeMutex() {
    pthread_mutex_t *p = new pthread_mutex_t;
    if (pthread_mutex_init(p, 0) != 0) {
        delete p;
        return 0;
    }
    return p;
}

//获取table实例
table *table::GetTable() {

    /**
    只允许构建一个table实例，因为对表的操作是互斥的
    **/

    if (m_table != 0) {
        return m_table;
    }
    if (pthread_mutex_lock(m_pMutexForCreatingTable) != 0) {
        return 0;
    }
    if (m_table == 0) {
        try {
            m_table = new table;
        } catch (const char *) {
            pthread_mutex_unlock(m_pMutexForCreatingTable);
            return 0;
        }
    }
    if (pthread_mutex_unlock(m_pMutexForCreatingTable) != 0) {
        return 0;
    }
    return m_table;
}

//创建一条随机记录并保存到记录数组中
Record table::CreateRecord() {
    Record record;
    record_num++;
    record.primary_key = record_num;
    for (int i = 0; i < RECORD_LENGTH; i++)
        record.record_array[i] = rand() % 2000; //属性是随机数，设置范围[0,1000]
    records[record_num - 1] = record;
    return record;
}

//保存一条记录
bool table::AppendRecord(const Record & record) {
    if (write(m_Fd, &record, RECORD_SIZE_BYTE) == -1)
        return false;
    return true;
}


//为第col列的属性创建B+树
BPlusTreeNode* table::CreateBPlusTree(int col) {
    BPlusTreeNode* root = tree->BPlusTree_create();
    for (int i = 0; i < record_num; i++)
        root = tree->BPlusTree_Insert(root, records[i], col);
    return root;
}

//更新索引文件
void table::UpdateIndexFile(int col) {
    BPlusTreeNode* root = CreateBPlusTree(col);
    tree->WriteBPlusTree(root, col);
}

//创建索引文件
void table::CreateIndexFile(BPlusTreeNode * root, int col) {
    tree->WriteBPlusTree(root, col);
}

//判断索引文件是否存在
bool table::Is_Index_File_Exists(int col) {
    char index_file_path[20];
    sprintf(index_file_path, "%s%d", INDEX_FILE_PATH, col);
    if (access(index_file_path, F_OK) == 0)
        return true;
    return false;
}


//插入记录
void table::InsertRecord() {
    //P(mutex)
    if (pthread_mutex_lock(m_pMutexForOperatingTable) != 0)
        throw "In table::InsertRecord(),lock error";
    Record record = CreateRecord();
    //DisplayRecord(records[record_num-1]);
    if (!AppendRecord(record))
        throw "In table::InsertRecord(),insert error";
    std::cout << "已成功添加一条记录：" << std::endl;
    DisplayRecord(record);
    //更新索引
    for (int col = 1; col < RECORD_LENGTH + 1; col++) {
        if (Is_Index_File_Exists(col))
            UpdateIndexFile(col);
    }
    //V(mutex)
    pthread_mutex_unlock(m_pMutexForOperatingTable);
}

//搜索记录
void table::SearchRecord(int left, int right, int col) {
    std::cout << "正在搜索第" << col << "列，范围：[" << left << "," << right << "]" << std::endl;
    //P(mutex)
    pthread_mutex_lock(m_pMutexForOperatingTable);

    //存放搜索结果
    int64_t *result = new int64_t[MAX_RESULT_NUM];
    int num = 0;
    BPlusTreeNode *root;//存放B+树
    //首先判断是否有索引文件
    if (Is_Index_File_Exists(col)) {
        std::cout << "已查找到索引文件，正在读取..." << std::endl;
        root = tree->ReadBPlusTree(col);
        if (!root) {
            std::cout << "读取索引文件失败" << std::endl;
            return ;
        }

    } else {
        std::cout << "未查找到索引文件，正在创建..." << std::endl;
        root = CreateBPlusTree(col);
        if (!root) {
            std::cout << "创建索引文件失败";
            return;
        }
        //创建索引文件
        CreateIndexFile(root, col);
        std::cout << "已为第" << col << "列创建索引文件" << std::endl;
    }

    if (left == right) {
        tree->SearchValueEqual(root, left, result, num);
    } else {
        tree->SearchValueRange(root, left, right, result, num);
    }
    std::cout << "搜索结果为：" << std::endl;
    for (int i = 0; i < num; i++)
        DisplayRecord(records[result[i] - 1]);
    //V(mutex)
    pthread_mutex_unlock(m_pMutexForOperatingTable);
}

//显示记录
void table::DisplayRecord(const Record & record) {
    std::cout << "--------------------------------------------------" << std::endl;
    for (int i = 0; i < 10; i++) {//方便展示，只显示记录的前10个属性
        printf("%-5ld", record.record_array[i]);
    }
    std::cout << std::endl;
}























