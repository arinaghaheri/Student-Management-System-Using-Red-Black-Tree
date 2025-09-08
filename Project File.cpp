#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <functional>
#include <fstream>

using namespace std;

enum RBColor { RED, BLACK };

template<typename Key, typename Val, typename Cmp = std::less<Key>>
class RBTree {
    struct Node {
        Key key;
        Val val;
        RBColor color;
        Node *left, *right, *parent;
        Node(const Key& k, const Val& v, RBColor c, Node* nil)
            : key(k), val(v), color(c), left(nil), right(nil), parent(nil) {}
    };

    Cmp cmp;
    Node* root;
    Node* NIL; 

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == NIL) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == NIL) root = x;
        else if (y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void insertFix(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* u = z->parent->parent->right; 
                if (u->color == RED) {
                    z->parent->color = BLACK;
                    u->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) { z = z->parent; leftRotate(z); }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                Node* u = z->parent->parent->left; 
                if (u->color == RED) {
                    z->parent->color = BLACK;
                    u->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) { z = z->parent; rightRotate(z); }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == NIL) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }

    Node* minimum(Node* x) const {
        while (x->left != NIL) x = x->left;
        return x;
    }

    void eraseFix(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    Node* findNode(const Key& k) const {
        Node* cur = root;
        while (cur != NIL) {
            if (cmp(k, cur->key)) cur = cur->left;
            else if (cmp(cur->key, k)) cur = cur->right;
            else return cur;
        }
        return nullptr;
    }

    void clearRec(Node* n) {
        if (n == NIL) return;
        clearRec(n->left);
        clearRec(n->right);
        delete n;
    }

    template<class F>
    void inorderRec(Node* n, F&& fn) {
        if (n == NIL) return;
        inorderRec(n->left, fn);
        fn(n->key, n->val);
        inorderRec(n->right, fn);
    }

public:
    RBTree(): cmp(Cmp()) {
        NIL = new Node(Key{}, Val{}, BLACK, nullptr);
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
    }
    ~RBTree() {
        clearRec(root);
        delete NIL;
    }

    bool insertUnique(const Key& k, const Val& v) {
        Node* y = NIL;
        Node* x = root;
        while (x != NIL) {
            y = x;
            if (cmp(k, x->key)) x = x->left;
            else if (cmp(x->key, k)) x = x->right;
            else return false;
        }
        Node* z = new Node(k, v, RED, NIL);
        z->parent = y;
        if (y == NIL) root = z;
        else if (cmp(k, y->key)) y->left = z;
        else y->right = z;
        insertFix(z);
        return true;
    }

    bool erase(const Key& k) {
        Node* z = findNode(k);
        if (!z) return false;

        Node* y = z;
        Node* x;
        RBColor y_orig = y->color;

        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_orig = y->color;
            x = y->right;
            if (y->parent == z) x->parent = y;
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (y_orig == BLACK) eraseFix(x);
        return true;
    }

    bool contains(const Key& k) const { return findNode(k) != nullptr; }

    bool get(const Key& k, Val& out) const {
        Node* n = findNode(k);
        if (!n) return false;
        out = n->val;
        return true;
    }
    Val* getPtr(const Key& k) {
        Node* n = findNode(k);
        return n ? &n->val : nullptr;
    }

    template<class F>
    void forEachInOrder(F&& fn) {
        inorderRec(root, std::forward<F>(fn));
    }

    bool moveKey(const Key& oldK, const Key& newK) {
        if (!contains(oldK) || contains(newK)) return false;
        Val v = *getPtr(oldK);
        erase(oldK);
        return insertUnique(newK, v);
    }
};

struct Student {
    int stu_id;
    string nat_id;
    string name;
    string family;
    double gpa;
};

class StudentSystem {
    RBTree<int, Student> byStu;      
    RBTree<string, int> idIndex;     
    int nextStuId = 1000001;         

    static string trim(const string &s) {
        size_t i = 0, j = s.size();
        while (i < j && isspace((unsigned char)s[i])) ++i;
        while (j > i && isspace((unsigned char)s[j-1])) --j;
        return s.substr(i, j - i);
    }
    static void printStudent(const Student& st) {
        cout << "ID: " << st.nat_id << ",\n"
             << "NAME: " << st.name << ",\n"
             << "FAMILY: " << st.family << ",\n"
             << "STU_ID: " << st.stu_id << ",\n"
             << "GPA: " << fixed << setprecision(2) << st.gpa << "\n";
    }

    bool parseRegister(const string &rest, Student &out) {
        vector<string> parts;
        string token; stringstream ss(rest);
        while (getline(ss, token, ',')) parts.push_back(trim(token));

        string name, family, nid; double gpa = NAN; bool gpa_seen=false;
        for (auto &p : parts) {
            auto pos = p.find(':'); if (pos==string::npos) continue;
            string k = trim(p.substr(0,pos));
            string v = trim(p.substr(pos+1));
            if (k=="NAME") name=v;
            else if (k=="FAMILY") family=v;
            else if (k=="ID") nid=v;
            else if (k=="GPA") { try { gpa=stod(v); gpa_seen=true; } catch(...){} }
        }
        if (name.empty() || family.empty() || nid.empty() || !gpa_seen) return false;
        out.name=name; out.family=family; out.nat_id=nid; out.gpa=gpa;
        return true;
    }

public:
    void cmdRegister(const string &rest) {
        Student s{};
        if (!parseRegister(rest, s)) { cout << "FAILED => BAD INPUT\n"; return; }
        if (idIndex.contains(s.nat_id)) { cout << "FAILED => RECORD EXISTS\n"; return; }

        s.stu_id = nextStuId++;
        byStu.insertUnique(s.stu_id, s);
        idIndex.insertUnique(s.nat_id, s.stu_id);
        cout << "SUCCESS => STU_ID:" << s.stu_id << "\n";
    }

    void cmdFind(const string &arg) {
        string t = trim(arg);
        if (t=="BESTGPA") {
            const Student* best = nullptr;
            byStu.forEachInOrder([&](const int&, Student& s){
                if (!best || s.gpa > best->gpa ||
                    (fabs(s.gpa - best->gpa) < 1e-12 && s.stu_id < best->stu_id))
                    best = &s;
            });
            if (!best) { cout << "FAILED => NOT FOUND\n"; return; }
            printStudent(*best);
            return;
        }
        try {
            int sid = stoi(t);
            if (auto p = byStu.getPtr(sid)) printStudent(*p);
            else cout << "FAILED => NOT FOUND\n";
        } catch(...) { cout << "FAILED => NOT FOUND\n"; }
    }

    void cmdFindGE(const string &arg) {
        double x;
        try { x = stod(trim(arg)); } catch(...) { cout << "FAILED => BAD INPUT\n"; return; }
        bool any = false;
        byStu.forEachInOrder([&](const int&, Student& s){
            if (s.gpa + 1e-12 >= x) {
                if (any) cout << "------------------------\n";
                printStudent(s);
                any = true;
            }
        });
        if (!any) cout << "FAILED => NOT FOUND\n";
    }

    void cmdDelete(const string &arg) {
        string t = trim(arg);
        if (t=="SUSPENDED") {
            vector<pair<int,string>> toDel;
            byStu.forEachInOrder([&](const int& k, Student& s){
                if (s.gpa + 1e-12 < 12.0) toDel.push_back({k, s.nat_id});
            });
            if (toDel.empty()) { cout << "FAILED => NOT FOUND\n"; return; }
            cout << "SUCCESS, DELETED RECORDS:\n";
            for (auto &pr: toDel) {
                int sid = pr.first; string nid = pr.second;
                Student st; if (byStu.get(sid, st)) {
                    cout << "NAME:" << st.name
                         << ", FAMILY:" << st.family
                         << ", ID:" << st.nat_id
                         << ", GPA:" << fixed << setprecision(2) << st.gpa << "\n";
                }
            }
            for (auto &pr: toDel) { idIndex.erase(pr.second); byStu.erase(pr.first); }
            return;
        }
        try {
            int sid = stoi(t);
            Student st;
            if (!byStu.get(sid, st)) { cout << "FAILED => NOT FOUND\n"; return; }
            idIndex.erase(st.nat_id);
            byStu.erase(sid);
            cout << "SUCCESS\n";
        } catch(...) { cout << "FAILED => NOT FOUND\n"; }
    }

    void cmdChange(const string &rest) {
        stringstream ss(rest);
        string sSid, attr; if (!(ss >> sSid >> attr)) { cout << "FAILED => BAD INPUT\n"; return; }
        string dst; getline(ss, dst); dst = trim(dst);

        int sid;
        try { sid = stoi(sSid); } catch(...) { cout << "FAILED => NOT FOUND\n"; return; }
        auto st = byStu.getPtr(sid);
        if (!st) { cout << "FAILED => NOT FOUND\n"; return; }

        if (attr == "ID") {
            int newSid;
            try { newSid = stoi(dst); } catch(...) { cout << "FAILED => BAD INPUT\n"; return; }
            if (!byStu.moveKey(sid, newSid)) { cout << "FAILED => RECORD EXISTS\n"; return; }
            if (auto p = byStu.getPtr(newSid)) p->stu_id = newSid;
            cout << "SUCCESS\n";
            return;
        }
        if (attr == "NAME")   { st->name = dst; cout << "SUCCESS\n"; return; }
        if (attr == "FAMILY") { st->family = dst; cout << "SUCCESS\n"; return; }
        if (attr == "GPA") {
            try { st->gpa = stod(dst); cout << "SUCCESS\n"; }
            catch(...) { cout << "FAILED => BAD INPUT\n"; }
            return;
        }
        cout << "FAILED => BAD INPUT\n";
    }

    void repl(istream &in) {
        string line;
        while (true) {
            if (!getline(in, line)) break;
            line = trim(line);
            if (line.empty()) continue;

            if      (line.rfind("REGISTER ", 0) == 0) cmdRegister(trim(line.substr(9)));
            else if (line.rfind("FIND_GE ", 0)  == 0) cmdFindGE(trim(line.substr(8)));
            else if (line.rfind("FIND ", 0)     == 0) cmdFind(trim(line.substr(5)));
            else if (line.rfind("DELETE ", 0)   == 0) cmdDelete(trim(line.substr(7)));
            else if (line.rfind("CHANGE ", 0)   == 0) cmdChange(trim(line.substr(7)));
            else if (line == "EXIT") break;
            else {
                cout << "Unknown command.\n";
            }
        }
    }
    void saveToFile(const string &filename) {
        ofstream out(filename);
        if (!out) return;
        byStu.forEachInOrder([&](const int&, Student& s){
            out << s.stu_id << ","
                << s.nat_id << ","
                << s.name << ","
                << s.family << ","
                << s.gpa << "\n";
        });
    }

    void loadFromFile(const std::string &filename) {
        std::ifstream in(filename);
        if (!in) return;

        std::string line;
        while (std::getline(in, line)) {  
            std::stringstream ss(line);
            std::string sid, nid, name, family, gpaStr;

            if (!std::getline(ss, sid, ',')) continue;
            if (!std::getline(ss, nid, ',')) continue;
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, family, ',')) continue;
            if (!std::getline(ss, gpaStr, ',')) continue;

            Student s;
            s.stu_id = std::stoi(sid);
            s.nat_id = nid;
            s.name = name;
            s.family = family;
            s.gpa = std::stod(gpaStr);

            byStu.insertUnique(s.stu_id, s);
            idIndex.insertUnique(s.nat_id, s.stu_id);

            nextStuId = std::max(nextStuId, s.stu_id + 1);
        }

    };
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    StudentSystem sys;
    
    sys.loadFromFile("students.txt");
    sys.repl(std::cin);
    sys.saveToFile("students.txt");
    
    return 0;
};
