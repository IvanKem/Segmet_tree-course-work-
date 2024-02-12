#include <climits>
#include <cstdint>
#include <iostream>
#include <bits/stdc++.h>
#include <type_traits>
#include <vector>
#include <cstdint>
#include <limits>
 
 
using namespace std;
int MOD =1e9+7;

 
// Функции для дерева отрезков
int Sum(int a, int b) {
    return a + b;
}

int Max(int a, int b){
    if (a>=b) return a;
    else return b;
}

int Min(int a, int b){
    if (a<=b) return a;
    else return b;
}

int Set(int a, int b){
    return b;
}

 
template<class T>
class SegmentTree {
protected:
    vector<T> tree; // массив, представляющий дерево отрезков
    int size; // размер исходного массива
    
    void init(int n){
        size = 1;
        while (size<n) size*=2;
        tree.assign(2*size-1,0);   
    }
    
    // запрос модификации. Ему точно так же передаётся информация о текущей вершине 
    // дерева отрезков, а дополнительно указывается индекс меняющегося элемента, а также его новое значение.
    virtual void update(T (*func)(T t1,T t2),int i , int v ,int x, int lx, int rx){
        if (rx-lx==1){
            tree[x]=v;
            return;
        }
        int m = (lx+rx)/2;
        if (i<m){
            update(func,i,v,2*x+1,lx,m);
        } else {
            update(func,i,v,2*x+2,m,rx);
        }
        tree[x]=func(tree[2*x+1],tree[2*x+2]);
    }


    // Рекурсивная функция для построения дерева отрезков
    virtual void build(vector<T>& arr,T (*func)(T t1,T t2), int x, int lx, int rx){
        if (rx-lx==1){
            if (lx<arr.size())
                tree[x]=arr[lx];
        } else {
            int m = (rx+lx)/2;
            build(arr,func,2*x+1,lx,m);
            build(arr,func,2*x+2,m,rx);
            tree[x]=func(tree[2*x+1],tree[2*x+2]);
        }
    }

    virtual void build(vector<T>& arr,T (*func)(T t1,T t2)) {
        init(arr.size());
        build(arr,func,0,0,size);
    }
 
 
    // Вычисление значения функции на отрезке (рекурсивная функция)
    T segment_query(T (*func)(T t1,T t2),int left, int right, int x, int lx, int rx) {
        if ((left >= rx|| lx>=right) && (func==Sum)) {
            return 0;   // Возвращаем нейтральное значение для операции (например, для суммы это 0)
        }
        if ((left >= rx|| lx>=right) && (func==Min)) {
            return 1e9;   // Возвращаем нейтральное значение для операции 
        }
        if ((left >= rx|| lx>=right) && (func==Max)) {
            return -1e9;   // Возвращаем нейтральное значение для операции 
        }

        if (lx>=left && rx<=right) {
            return tree[x];  // Ответ находится в текущем узле
        }
 
        int mid = (rx + lx) / 2;
        // Рекурсивно спускаемся в левое поддерево
        T leftResult = segment_query(func, left,right,2*x+1,lx,mid );       
        // Рекурсивно спускаемся в правое поддерево    
        T rightResult = segment_query(func, left,right,2*x+2, mid ,rx);    
        return func(leftResult, rightResult);    // Объединяем результаты левого и правого поддеревьев
    }
    
    
public:
   
    // Конструктор класса
    SegmentTree() {}
    SegmentTree(vector<T>& arr, T (*func)(T t1,T t2)) {
        build(arr,func); // построение дерева отрезков
    }
 
    T segment_query(T (*func)(T t1,T t2),int left, int right){
        return segment_query(func, left, right,0,0,size);
    } 

    virtual void update(T (*func)(T t1,T t2),int i, int v){
        update(func,i,v,0,0,size);
    }
    const vector<T> &get_ST() const {return tree;}
    int get_n(){return size;}
 
    virtual void Print(){
        vector<T> v = tree;
        for (int n : v){cout << n << ' ';};
        cout<<endl;
    }
    
 
}; 
 
 
template<class T>
class Segment_group_tree : public SegmentTree<T> {
private:
    T NO_OPERATION= numeric_limits<T>::min();
    
    //Прибавление на отрезке
    void modify(T (*func)(T t1,T t2),int l, int r, int v, int x, int lx, int rx){
        if (l >= rx|| lx>=r){
            return;   
        }
        if (lx>=l && rx<=r) {
            SegmentTree<T>::tree[x]=func(SegmentTree<T>::tree[x], v); 
            return; 
        }
        int mid = (rx + lx) / 2;
        // Рекурсивно спускаемся в левое поддерево
        modify(func,l,r,v,2*x+1,lx,mid);       
        // Рекурсивно спускаемся в правое поддерево    
        modify(func,l,r,v,2*x+2,mid,rx);     
    }

    T get(T (*func)(T t1,T t2),int i ,int x, int lx, int rx){
        if (rx-lx==1){
            return SegmentTree<T>::tree[x];
        }
        int m = (lx+rx)/2;
        if (i<m){
            return func(get(func,i,2*x+1,lx,m),SegmentTree<T>::tree[x]);
        } else {
            return func(get(func,i,2*x+2,m,rx),SegmentTree<T>::tree[x]);
        }
    }

    //Проталкивание
    void propagate(int x, int lx, int rx){
        if (SegmentTree<T>::tree[x] == NO_OPERATION) return;
        if (rx-lx == 1) return;
        SegmentTree<T>::tree[2*x+1]= SegmentTree<T>::tree[x];
        SegmentTree<T>::tree[2*x+2]=SegmentTree<T>::tree[x];
        SegmentTree<T>::tree[x]=NO_OPERATION;
    }

    //групповая операция для некоммутативных функций
    void lazy_modify(int l, int r, int v, int x, int lx, int rx){
        propagate(x,lx,rx);
        if (l >= rx|| lx>=r){
            return;   
        }
        if (lx>=l && rx<=r) {
            SegmentTree<T>::tree[x]= v; 
            return; 
        }
        int mid = (rx + lx) / 2;
        // Рекурсивно спускаемся в левое поддерево
        lazy_modify(l,r,v,2*x+1,lx,mid);       
        // Рекурсивно спускаемся в правое поддерево    
        lazy_modify(l,r,v,2*x+2,mid,rx);     
    }

    T lazy_get(int i ,int x, int lx, int rx){
        propagate(x, lx, rx);
        if (rx-lx==1){
            return SegmentTree<T>::tree[x];
        }
        int m = (lx+rx)/2;
        T res;
        if (i<m){
            res = lazy_get(i,2*x+1,lx,m);
            
        } else {
            res = lazy_get(i,2*x+2,m,rx);
        }
        return res;
    }  
    
public: 
    
    Segment_group_tree(vector<T>& arr, T (*func)(T T1, T T2)):SegmentTree<T>(arr, func){};
 
    void modify(T (*func)(T t1,T t2),int l, int r, int v){
        return modify(func,l,r,v,0,0,SegmentTree<T>::size);
    }
    T get(T (*func)(T t1,T t2),int i){
        return get(func,i, 0, 0, SegmentTree<T>::size);
    }
    //групповая операция для некоммутативных функций
    void lazy_modify(int l, int r, int v){
        return lazy_modify(l, r, v, 0, 0, SegmentTree<T>::size);
    }

    T lazy_get(int i){
        return lazy_get(i, 0, 0, SegmentTree<T>::size);
    }  
    
 
};    

 
using namespace std;

template <class T>
class Segment_Lazy_tree : public SegmentTree<T>{
public:
    struct node{
        T set; //хранение модификаций
        T min; //хранения значение функции
    };
    
    T NO_OPERATION=get_min_value();
    T NEUTRAL_ELEMENT=get_max_value();

    vector<node> tree;
    int size;

    vector<bool> lazy;

    T get_min_value() {
        return numeric_limits<T>::min();
    }
    T get_max_value() {
        return numeric_limits<T>::max();
    }
    //операция изменяющаяя отрезок
    T op_modify(T a,T b, T len){
        if (b==NO_OPERATION) return a;
        return b;
    }
    //операция запроса на отрезке
    T op_sum(T a,T b){
        return ::min(a,b);
    }
     // Рекурсивные функции для построения дерева отрезков
    
    void init(int n){
        size = 1;
        while (size<n) size*=2;
        tree.resize(2*size-1);   
    }

    void build(vector<T>& arr, int x, int lx, int rx) {
        if (rx-lx==1){
            if (lx<arr.size()) {
                tree[x].min = arr[lx];
                tree[x].set = NO_OPERATION;
            }
            return;
        }

        int mid = (lx + rx) / 2;
        build(arr, 2 * x + 1, lx, mid);
        build(arr, 2 * x + 2, mid, rx);

        tree[x].min = op_sum(tree[2 * x + 1].min, tree[2 * x + 2].min);
        tree[x].set = NO_OPERATION;
    }
    void build(vector<T>& arr){
        init(arr.size());
        build(arr,0,0,size);

    }    


    //Проталкивание
    void propagate(int x, int lx, int rx){
        if (tree[x].set == NO_OPERATION || rx-lx==1) return;
        int m = (lx+rx)/2;
        tree[2*x+1].set = op_modify(tree[2*x+1].set,tree[x].set,1);
        tree[2*x+1].min = op_modify(tree[2*x+1].min,tree[x].set,m-lx);
        tree[2*x+2].set = op_modify(tree[2*x+2].set,tree[x].set,1);
        tree[2*x+2].min = op_modify(tree[2*x+2].min, tree[x].set,rx-m);
        tree[x].set=NO_OPERATION;      
    }
    
   
    //Присваивание на отрезке (любая операция изменяющая отрезок)
    void mult(int l, int r, T v, int x, int lx, int rx){
        propagate(x, lx, rx);
        if (l >= rx || lx>=r){
            return;   
        }
        if (lx>=l && rx<=r) {
            tree[x].set = op_modify(tree[x].set,v,1);
            tree[x].min = op_modify(tree[x].min,v,rx-lx);
            
            return; 
        }
        int mid = (rx + lx) / 2;
        // Рекурсивно спускаемся в левое поддерево
        mult(l, r, v,2*x+1, lx,mid);       
        // Рекурсивно спускаемся в правое поддерево    
        mult(l, r, v,2*x+2,mid, rx);  
        tree[x].min=op_sum(tree[2*x+1].min,tree[2*x+2].min);

    }
    

    //Запрос на отрезке
    T sum(int l, int r, int x, int lx, int rx){
        propagate(x, lx, rx);
        if (l >= rx || lx>=r){
            return NEUTRAL_ELEMENT;   
        }
        if (lx>=l && rx<=r) {
            
            return tree[x].min; 
        }
        int mid = (rx + lx) / 2;
        // Рекурсивно спускаемся в левое поддерево
        T m1 = sum(l, r, 2*x+1, lx,mid);       
        // Рекурсивно спускаемся в правое поддерево    
        T m2 = sum(l, r, 2*x+2,mid, rx); 
        return op_sum(m1,m2);
        
    }

    
    
public: 
    Segment_Lazy_tree<T>(vector<T>& arr) : lazy(size,false){
        build(arr); // построение дерева отрезков
    }
    
 
    void mult(int l, int r, int v){
        return mult(l,r,v,0,0,size);
    }
    
    T sum(int l, int r){
        return sum(l,r,0,0,size);
    }
    void Print(){
        vector<node> v = tree;
        for (auto n : v){cout << "{"<<n.set<<","<<n.min<<"}" << ' ';};
        cout<<endl;
    }
    
}; 

 
 
 


 
 
 
 
int main() {
    
    /*vector<int> arr = {1, 3, 5, 7, 9, 11,5,2};    // Пример входного массива
    
    cout<<"Исходный массив : ";
    for (auto n : arr){cout << n << ' ';};
    cout<<endl;
    
    cout<<"Построенное дерево на максимум : ";
    SegmentTree<int> segTree(arr,Max); // Создаем дерево отрезков на основе массива и любой ассоциативной функии
    segTree.Print();
    cout<<"Значение максимума на отрезке от 1 до 4 : ";
    cout<<segTree.segment_query(Max, 1,4)<<endl;
    segTree.update(Max, 5, 1);
    cout<<"Изменяем значение дерева по индексу 5 на число 1 : ";
    segTree.Print();
    */

 
    //Групповое ДО
    /*
    Segment_group_tree<int> seg_g_Tree(arr,Sum);
    cout<<"Построенное дерево отрезков на сумму : ";
    seg_g_Tree.Print();
    cout<<"Присвоение числа 5 на отрезке от 2 до 8"<<endl;
    seg_g_Tree.lazy_modify(2, 8, 5);
    cout<<"Значение на 5ой позиции : ";
    cout<<seg_g_Tree.lazy_get(5)<<endl;
    */

    //Групповое ДО с отложенными оперциями
    
    ios::sync_with_stdio(false);
    int n,m;
    cin>>n>>m;
    vector<int> arr(n,0);
    Segment_Lazy_tree<int> segTree(arr);
    cout<<"Исходный массив : ";
    for (auto n : arr){cout << n << ' ';};
    cout<<endl;
    
    for (int t = 0; t < m ;t++){
        int c; 
        cin >> c;
        if (c==1){
            
            int l,r,v;
            cin>>l>>r>>v;
            cout<<"Запрос обновления на отрезке, в данном случае присваивание числа "<<v<<" на отрезке ";
            cout<<"["<<l<<", "<<r<<")"<<endl;
            segTree.mult(l,r,v);

        } else {
            int l,r;
            cin >> l >> r;
            cout<<"Запрос функции на отрезке, в данном случае минимум на отрезке ";
            cout<<"["<<l<<", "<<r<<") -> ";
            cout << segTree.sum(l, r) << "\n";
            
        }

    }
    
    return 0;
}
