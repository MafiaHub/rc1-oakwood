# How to add natives

This guide showcases how to add natives to the server.

The first step is to open `Code/Server/natives.hpp` and browse to a particular section you want to add native at. Adding a native method is as simple as writing a function that gets preceded by `OAKGEN_NATIVE();` decorator, such as:

```cpp
OAKGEN_NATIVE();
void oak_foo() {
    // code
}
```

Natives are always prefixed with `oak_` keyword, following a namespace (such as: `player_` or none if unspecified). Once a native method has been implemented, navigate to the source Oakwood repository folder and execute `Scripts\generate_api.bat`, which will do the rest of the magic for you. Don't forget to test your natives and make sure they are covered by the Oakwood Framework as well (if applicable.)

Our generators will amalgamate all the natives and generate the required interface, so that you can use the API in your plugins or Oakwood Framework-powered game modes.

For more information, contact ZaKlaus.
