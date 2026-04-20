#include<iostream>
#include<fstream>

using namespace std;

class Entry {
public:
  Entry(){
    valid = false;
    tag = 0;
    ref = 0;
  }
  ~Entry(){}
  void display(ofstream& outfile){
    outfile << valid << " " << tag << " " << ref;
  }

  void set_tag(int _tag) { tag = _tag; }
  int get_tag() { return tag; }

  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }

  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

class Cache {
public:
  Cache(int _num_entries, int _assoc){
    num_entries = _num_entries;
    assoc = _assoc;
    num_sets = num_entries / assoc;

    entries = new Entry*[assoc];

    for (int way = 0; way < assoc; way++) {
        entries[way] = new Entry[num_sets];
    }
  }

  ~Cache(){ 
    for (int way = 0; way < assoc; way++) {
        delete[] entries[way];
    }

    delete[] entries;
  }

  void display(ofstream& outfile){
    for (int set = 0; set < num_sets; set++) {
    outfile << "Set " << set << ": ";

    for (int way = 0; way < assoc; way++) {
        entries[way][set].display(outfile);
    }

    outfile << endl;
    } 
  }

  int get_index(unsigned long addr) { return addr % num_sets; }
  int get_tag(unsigned long addr) { return addr / num_sets; }

  unsigned long retrieve_addr(int way, int index){
    return entries[way][index].get_tag() * num_sets + index;
  }
  
  bool hit(ofstream& outfile, unsigned long addr){
    int index = get_index(addr);
    int tag = get_tag(addr);

    for (int way = 0; way < assoc; way++) {
        if (entries[way][index].get_valid() &&
            entries[way][index].get_tag() == tag) {

            outfile << addr << " : HIT" << endl;
            return true;
        }
    }

    outfile << addr << " : MISS" << endl;
    return false;
  }

  void update(ofstream& outfile, unsigned long addr){
    int index = get_index(addr);
    int tag = get_tag(addr);

    for (int way = 0; way < assoc; way++) {
        if (!entries[way][index].get_valid()) {
            entries[way][index].set_valid(true);
            entries[way][index].set_tag(tag);
            entries[way][index].set_ref(counter++);
            return;
        }
    }

    // using FIFO
    int replace_way = 0;
    int smallest = entries[0][index].get_ref(); // addr that went in first

    for (int way = 1; way < assoc; way++) {
        if (entries[way][index].get_ref() < smallest) {
            smallest = entries[way][index].get_ref();
            replace_way = way;
        }
    }

    entries[replace_way][index].set_tag(tag);
    entries[replace_way][index].set_ref(counter++);
    entries[replace_way][index].set_valid(true);
}

  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;
  int counter; // tracks addr using FIFO
};

