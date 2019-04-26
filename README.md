# Function2

[![Travis CI][shields.io]][travis-ci.com]

Function2 is a C++17 template class to wrap any invocable object.

## Example

```c++
#include <fn2/fn2.h>

#include <cassert>
#include <cctype>
#include <string>

static std::string::size_type num_spaces(const std::string &str) noexcept;

int main() {
    fn2::Function<std::string::size_type(const std:string&)> f(&std::string::size);

    const std::string str = "Hello, world!";

    assert(f(str) == 13);

    f = num_spaces;

    assert(f(str) == 1);
}

static std::string::size_type num_spaces(const std::string &str) noexcept {
    return std::count_if(
        str.cbegin(), str.cend(), [](char ch) { return std::isspace(ch); }
    );
}
```

## Installation

```sh
git clone https://github.com/Gregory-Meyer/function2.git
mkdir function2/build
cd function2/build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j `nproc`
cmake --build . --target test
sudo cmake --build . --target install
```

## License

Function2 is licensed under the MIT license.

[travis-ci.com]: https://travis-ci.com/Gregory-Meyer/function2/
[shields.io]: https://img.shields.io/travis/com/Gregory-Meyer/function2.svg
