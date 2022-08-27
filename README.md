## Onegin sorter

Read file in CP1251 and write it into another file in three versions:
1. Sorted by lines
2. Original form
3. Sorted by reverse lines (without changing lines)

### How to build
```bash
git clone --depth=1 https://github.com/foxidokun/onegin.git
cd onegin
make
```
And then
```bash
./bin/onegin -h #for short help message
./bin/onegin <input file> <output file>
#OR
make run # aka ./bin/onegin in.txt out.txt
```

### How to run tests
```bash
make test
```
This command will run all tests and create a binary file ./bin/onegin_test which will do the same.

### How to generate docs
```bash
cd onegin
doxygen
```
Then you can open ./docs/html/index.html in your browser