# Enjoying graphs with Thomas H. Cormen and Donald E. Knuth

<p align="center">
  <img src="https://user-images.githubusercontent.com/71986226/154510384-f02decea-a93b-4cb4-9633-a6aa5c87d198.gif"/>
</p>

## How to install

```bash
git clone git@github.com:KetchuppOfficial/Graph_Course.git
cd Graph_Course
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Graph_Course/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```

Targets:

- **generator**: a program that generates a graph in a format discussed below and prints it to
stdout.

- **driver**: a program that receives a graph from stdin and runs DFS on that graph.

- **graphic_dump**: a program that receives a graph from stdin and produces the graph's description in dot format on stdout.

- **recursive_driver**: the same as **driver** except that DFS algorithm is recursive.

- **unit_tests**: self-explanatory.

If --target option is omitted, all targets will be built.

## How to run unit tests

```bash
ctest --test-dir build
```

## How to run end-to-end tests

1) If you want to run some tests on DFS, a special script [dfs.sh](/test/end-to-end/dfs.sh) is
   provided for such purpose.

   Let **V** be the number of vertices, **E** - the number of edges, then command sequence:

   ```bash
   ./test/end-to-end/dfs.sh V E
   ```

   generates a directed graph of **V** vertices and **E** edges. The graph is saved in
   **./test/end-to-end/data/V-E.test**. The format of such file is defined with the following regular
   expression:

   ```
   \s+((V\s+\d+)|(E(\s+\d+){2}-?\d+))+\s*
   ```

   Then, both recursive and non-recursive versions of DFS are run on the test and the execution time
   is printed to stdout.
