#include<cstdlib>
#include<iostream>
#include<string.h>
/*本类为LruCache类*/
using namespace std;
const int hashTableSize = 97;//hash桶的数量
template<class K, class V>
struct hash_Node //定义hash节点的结构体
{
    K key;
    V value;
    hash_Node *next;
    hash_Node(K k1, V v1):key(k1),value(v1),next(nullptr) {}
    hash_Node(const hash_Node&);
    hash_Node& operator=(const hash_Node&);
};

template<class K, size_t tablesize>
struct KeyHash //定义hash函数
{
    unsigned long operator()(const K& key) const //重载（）
    {
        return static_cast<unsigned long>(key)%tablesize;
    }
};
template<class K, class V,size_t table_size, class F = KeyHash<K,table_size>>
class hash_table
{
private:
    hash_Node<K,V>* table[table_size];//哈希桶的个数
    F hashFunc;//哈希函数
    hash_table& operator=(const hash_table& other);
public:
    hash_Node<K,V>* operator [](const K& k1) //必须要重载，要不然table[i]会报错
    {
        unsigned long hashK = hashFunc(k1);
        return table[hashK];
    }
    hash_table():table(),hashFunc()
    {
    }
    ~hash_table() //析构函数
    {
        cout<<"hash_table begin destruct"<<endl;
        for(size_t i = 0; i < table_size; i++)
        {
            hash_Node<K,V> *node = table[i];

            while(node != nullptr)
            {
                hash_Node<K, V> *prev = node;
                node = node->next;
                delete prev;
            }
            table[i] = nullptr;
        }
    }
    /*brief: hash_table 获取某个元素
    *param: k1 传入的k，根据K找到V
    *return: V 也就是hash_table的val。
    *author: 田汉文 data:2019-09-02
    */
    V get(const K& k1) //根据传入的key，返回alue
    {
        //1、计算key的hash值
        unsigned long hashkey = hashFunc(k1);
        hash_Node<K, V> *node = table[hashkey];
        while(node != nullptr)
        {
            if(node->key == k1) //如果有则返回
            {
                return node->value;
            }
            node = node->next;
        }
        return nullptr;//没有返回空值
    }
    /*brief: hash_table 放入某个元素
    *param: k1 传入的key，v1:传入的value
    *author: 田汉文 data:2019-09-02
    */
    void put(const K& k1, const V& v1) //本方法可以替换现有的key
    {
        //1先计算hash
        unsigned long hashK = hashFunc(k1);
        //2、根据hash找到元素
        hash_Node<K, V> *node = table[hashK];
        while(node != nullptr) //先遍历有没有重复的结点，重复的进行替换
        {
            if(node->key == k1)
            {
                node->value = v1;//找到了就把他置为当前的value
                return;
            }
            node = node->next;
        }
        //下面的情况是处理头插入,hash_table就是用的头插入法，也就是所谓的开链法
        hash_Node<K, V> *tmp = new hash_Node<K,V>(k1,v1);
        tmp->next = table[hashK];//先调整新建结点的
        table[hashK] = tmp;//再调整输入结点的
    }
    /*brief: hash_table移除某个元素
    *param: k1 传入的k，根据K找到V
    *author: 田汉文 data:2019-09-02
    */
    void remove(const K& k1)
    {
        unsigned long hashK = hashFunc(k1);//求keyd的hash值
        hash_Node<K, V> *node = table[hashK];
        hash_Node<K, V> *prev = nullptr;//要删除节点的前驱节点
        //检查node节点是否为空
        if(node == nullptr)//也就是刚开始就没有找到
            return;//如果刚开始就是空，就z直接返回
        while(node != nullptr && node->key != k1) //节点不为空，并且当前节点的key不相等
        {
            prev = node;
            node = node->next;//寻找下一个节点
        }
        if(node == nullptr)//如果遍历了一遍key没有找到
            return;//说明没有找到该元素
        else
        {
            if(prev == nullptr)//说明首元素就是要找的点
                table[hashK] = node->next;//直接让开始指向要找的下一个节点
            else
            {
                prev->next = node->next;//让前驱节点直接指向要删除节点的下一个节点
            }
            delete node;
            node = nullptr;
        }
    }
};

template<class K, class V>
class LRUCache
{
private:
    struct ListNode
    {
        K key;
        V val;
        ListNode *prev;//前驱结点
        ListNode *next;//后继节点
        ListNode():key(0),val(0),prev(nullptr),next(nullptr) {};
        ListNode(K k, V v):key(k),val(v),prev(nullptr),next(nullptr) {};
    };
    int capacity;//LRUCache的容量
    ListNode* dummyNode;//双向链表带头虚拟结点
    hash_table<K,ListNode*,hashTableSize> hashMap;//自定义的hash_table结点
    int tmpSize;//标志LRUCache的当前节点数

private:
    /*以下几个函数都不会被外界调用，因此声明为私有*/

    /*brief:向链表头插入节点
    *param node:链表节点的元素值，flag:标志 0代表的是从moveToHead调用的，不需要做增加tmpSize的大小。否则需要增加
    *author:田汉文 data:2019-09-03
    */
    void pushFront(ListNode* node,int flag)
    {
        if(node->prev == dummyNode)//如果就在首位置，直接返回
            return;
        node->next = dummyNode->next;//双向链表插入经典四步曲
        node->prev = dummyNode;
        dummyNode->next->prev = node;
        dummyNode->next = node;
        if(flag != 0)
            tmpSize++;
    }
    /*brief:把当前节点从链表中断链，然后再插入到第一个位置
    *author:田汉文 data:2019-09-03
    */
    void moveToHead(ListNode* node)
    {
        if(node->prev == dummyNode)
            return ;
        node->next->prev = node->prev;//双链表删除经典两步曲
        node->prev->next = node->next;
        pushFront(node,0); //把节点插入到链表头部,这个地方调用pushFront函数flag参数传入0
    }
    /*brief:弹出链表里面的最后一个位置，由于是循环链表，最后的位置为 dummyNode->prev
    *author:田汉文 data:2019-09-03
    */
    void popBack()
    {
        ListNode* tem = dummyNode->prev;
        hashMap.remove(tem->key);//hashMap中也删除相应的节点
        tem->prev->next = dummyNode;//删除两步曲
        dummyNode->prev = tem->prev;
        tmpSize--;
        delete tem;
    }

public:

    /*brief: LRUCache的构造函数
    *param: cap:缓存的初始化的容量的大小
    *author:  田汉文 data:2019-09-03
    */
    LRUCache(int cap)
    {
        capacity = cap;
        tmpSize = 0;
        //初始化dummyNode节点，前驱和后继都指向他自己。
        dummyNode = new ListNode();
        dummyNode->prev = dummyNode;
        dummyNode->next = dummyNode;
    }
    /*brief: LRUCache析构函数
    *author:  田汉文 data:2019-09-03
    */
    ~LRUCache()
    {
        cout<<"LRUcache begin destruct"<<endl;
        eraseAll();
    }
    /*brief: LRUCache根据K获取V的函数
    *param: key：要查询的主键值的大小
    *author:  田汉文 data:2019-09-02
    */
    V get(const K& key)
    {
        ListNode*  tmpNode = hashMap.get(key);
        if(tmpNode == nullptr)
            throw "Cache not found element!";
        V value = tmpNode->val;
        moveToHead(tmpNode);//元素移动到首位置
        return value;
    }
    /*brief: LRUCache放入元素的函数
    *param: key1：主键值， val1 值本身
    *author:  田汉文 data:2019-09-02
    */
    void put(const K& key1, const V& val1)
    {
        ListNode* tmpNode = hashMap.get(key1);
        if(tmpNode == nullptr)
        {
            if(tmpSize == capacity)//如果当前节点等于容量
            {
                //移除最后的元素
                popBack();
            }
            //头插入元素
            ListNode* frontNode = new ListNode(key1,val1);
            pushFront(frontNode,1);//插入链表头结点
            hashMap.put(key1,frontNode);//插入hashMa中
        }
        else//说明原来存在元素
        {
            tmpNode->val = val1;//val1值有可能是不同的
            hashMap.put(key1,tmpNode);//放入该元素，会替换相应的val
            moveToHead(tmpNode);
        }
    }
    /*brief: LRUCache中遍历元素的函数
    *author:  田汉文 data:2019-09-02
    */
    void showElement()
    {
        ListNode *tmp = dummyNode->next;
        for(int i = 0 ; i < tmpSize && tmp != nullptr; i++)
        {
            cout<<tmp->val<<" ";
            tmp = tmp->next;
        }
        cout<<endl;
    }
    /*brief: LRUCache中删除所有元素的函数
    *author:  田汉文 data:2019-09-02
    */
    void eraseAll()
    {
        while(dummyNode->next != dummyNode)
        {
            popBack();
        }
        tmpSize = 0;
    }
    /*brief:得到LRUCache当前缓存的大小
    return：Lrucache中当前元素个数。const
    *author:田汉文 data:2019-09-02
    */
    int getSize() const
    {
        return tmpSize;
    }
    /*brief:得到LRUCache当前容量的大小
    return：Lrucache中当前容量大小。const
    *author:田汉文 data:2019-09-02
    */
    int getCapacity() const
    {
        return capacity;
    }
};
int main()
{
    LRUCache<int,int> lruCache(5);
    lruCache.put(1,1);
    lruCache.put(2,2);
    lruCache.put(3,3);
    lruCache.put(4,4);
    lruCache.put(5,5);
    lruCache.put(6,6);
    cout<<"访问所有的元素"<<endl;
    lruCache.showElement();
    cout<<"访问4节点，4节点提前"<<endl;
    lruCache.get(4);
    lruCache.showElement();
    cout<<"清空元素后的大小"<<endl;
    lruCache.eraseAll();
    cout<<lruCache.getSize()<<endl;
    cout<<"执行完毕，调用析构函数"<<endl;
    return 0;
}
