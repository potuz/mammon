# Design

## Template for ssz_container + yaml parser

The `ssz::ssz_container` object needs to be able to access its children. Either by pointers that won't be deleted or by storing them as references.

The basic types need to override `virtual serialize()` and need to have no children.

To parse yaml we need to generate a struct that has the following properties. Here `T` is the type of the container.
1. The name of the structure is `YAML::convert<T>`
2. It should two static members `static Node encode(const T& t)` and another `static bool decode(const Node&, T& t)`
3. The encode part has the form `node["child"] = t.child`.
Here the probem is that we should have defined `YAML::convert<S>` where `S` is the type of `child`
4. The decode part has the form `t.child = node["child"].as<S>`, the same problem as above.

So the `Container` object needs to hold some structure like

```C++
struct child
{
  std::string name;
  ssz::Container& child;
};
```

And somehow have its type `S` available at construction time. The last child will have to explicitly set up its conversion as well.   

We can probably store a `static constexpr type = S` in the child's type. since this is evaluated at compile time.

## Cache of Partial Merkle-Tree
We need to keep a cache of the hash tree of the bigger lists. We are confronted with the following problem. Since `VALIDATOR_REGISTRY_LIMIT` is $2^{40}$ but we only have ~180K validators, our tree looks like

![Alt text](https://g.gravizo.com/source/custom_mark10?https%3A%2F%2Fraw.githubusercontent.com%2Fpotuz%2Fmammon%2Fmain%2Fassets%2Fdesign.md)
<details>
<summary></summary>
custom_mark10
digraph G {
  root [color=blue, style=bold]
  a [color=blue, style=bold]
  root -> a [color=blue];
  b [label ="#(#(0,0),#(0,0))",style=filled]
  root -> b
  c [label = "b"]
  d [label ="c",color=blue,style=bold]
  e [label = "#(0,0)", style=filled]
  f [label = "#(0,0)", style=filled]
  a -> c
  a -> d[color=blue]
  b -> e;
  b -> f;
  c -> 1;
  c -> 2;
  3
  4 [label ="0",color=blue,style=bold]
  5 [label ="0",style=filled]
  6 [label ="0",style=filled]
  7 [label ="0",style=filled]
  8 [label = "0",style=filled]
    d -> 3;
    d -> 4 [color=blue]
    e -> 5;
    e -> 6;
    f -> 7;
    f -> 8;
 }
custom_mark10
</details>

Here the right hand greyed-out part of the tree consists of a *zero-hash-array* that is constants. The `#(0,0)` means the hash of `32*'b0' + 32*'b0'` and recursively define the next levels

For some lists, for example the `validators` list, additions typically happen in the right and in that case a modification requires only knowing a few nodes. For example to add a fourth entry in the above diagram we only need to modify the path marked in blue and to compute the new path we only need the siblings, that is the nodes marked `3`, `b`, and the root of the zero-hash array. These are the ones that are kept for example in the deposit contract since no changes to the previous deposits is allowed.

However, changes to the validators (for example one may get slashed) or the effective balance, etc. Or even worse for the list of balances which changes on every epoch, require to change the entire non-zero part of the array. So for these lists we will keep a cache of the following data:

- The level of the whole tree: in this case `4`.
- The actual data array consisting in this case of `[root, a, b, c, 1, 2, 3, 0]` (note that it may be finalized by zero)
- The level of the zero-hash array on the right, in this case `3`
