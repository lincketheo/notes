# Notes
- Superset of javascript (all javascript is valid typescript)
- Strongly typed
- Compiles to javascript

# Compiling and Running
- Basic
```
$ cd simple
$ tsc app.ts --outDir js
$ node js/app.js
```

- Using Watch mode
```
$ tsc --outDir js --watch app.ts
```

- Projects
```
$ tsc --init
# Edit tscondig.json
$ tsc # Compile
```

# Syntax
- Let is strict
```
let x: string = "foo bar";
x = 42; // Compile error

let y = "biz baz";
y = 42; // Compile error
```

- Built in types
```
Void 
Null 
Undefined 
Never - never returns
Any
```

- Union Types
```
let val: number | string;
```

- By default null/undefined can be assigned to any type. Fix using:
    - `--strictNullChecks`
    - A generally good option

- Arrays, two ways:
```
let strarray: string[] = ['foo', 'bar', 'biz', 'buz'];
let strarray: Array<string> = ['foo', 'bar', 'biz', 'buz'];
```

# Functions
- Parameters default to any:
```
function foobar(value1, value2) { }
function foobar(value1: number, value2: string): string { }
```

- Unless you use `--noImplicitAny`
```
function foobar(value1, value2) { } // Compile error
```

- All parameters are required unless optional
- Appear after all required parameters
```
function createcustomer(name: string, age?: number) { } 
```

- Default Paramaters
```
function gettitle(title: string = "foobar") { }
```

- Rest Parameters 
```
function getbooks(name: string, ...bookIds: number[]) { }
getbooks("foo", 1, 2, 3);
```

- Arrow Functions 
- More than 1 param - surround with parenths
```
let square = x => x * x;
let adder = (a, b) => a + b;
let greeting = () => console.log("Hello there");
```

- Function overloading
```
// "Overload Signatures:"
function gettitles(author: string): string[];
function gettitles(available: boolean): string[];

function gettitles(prop: any): string[] {
    if(typeof prop == 'string') {
        
    }
    else if(typeof prop == "boolean") {

    }
}
```

# Interfaces
```
interface Duck {
    walk: () => void;
    swim: () => void;
    quack: () => void;
}

let kindofADuck = {
    walk: () => console.log("walk");
    swim: () => console.log("swim");
    quack: () => console.log("quack");
}

function fly(duck: Duck) {
    duck.walk();
    duck.swim();
    duck.quack();
}
```

- For function types 
```
function CreateMovieID(name: string, id: number): string {
    return name + id;
}
interface StringGenerator {
    (chars: string, nums: number): string;
}
let IdGenerator: StringGenerator = CreateMovieID;
```

- Extending interfaces 
```
interface LibResource{
    catnum: number;
}

interface Book {
    title: string;
}

interface Encyclopedia extends LibResource, Book {
    volume: number;
}

let refBook: Encyclopedia = {
    catnum: 45,
    title: "foo",
    volume: 41
}
```

- Implementing interfaces with classes 
```
interface Librarian {
    doWork: () => void;
}

class ElementaryLibrarian implements Librarian {
    doWork() {
        console.log("Reading to children");
    }
}

let kidsLibrarian: Librarian = new ElementaryLibrarian();
kidsLibrarian.doWork();
```

# Classes 
- Constructors
- only 1 per class
    - Use optional for traditional constructor overriding
```
class Item {
    constructor(title: string, publisher?: string) {
        // do stuff
    }
}

let thing = new Item("foo", "bar");
```

- Properties and Methods
- Types of properties
```
class Item {
    numPages: number;
    get editor(): string {
        // get editor
    }
    set editor(editor: string) {
        // set editor
    }
    printChapterTitle(num: number): void {
        // do stuff
    }
}
```

- Parameter Properties 
```
class Author {
    name: string 
    constructor(aname: string) {
        name = aname;
    }
}

// Same as

class Author {
    constructor(public name: string) { }
}
```

- Static properties 
```
class Library {
    constructor(public name: string) { }
    static description: string = 'A source of knowledge';
}
```

- Access Modifiers 
    - By default - all are public
    - private: `#`
    - protected

- Inheritance
```
class ReferenceItem {
    title: string;
    printItem(): void { }
}

class Journal extends ReferenceItem {
    constructor() { super(); }
    contributors: string[];
}
```

- Abstract classes 
    - Can only be used as base classes - can't directly instantiate abstract classes 
    - Difference from interface:
        - May contain implementation details
```
abstract class Foo {
    constructor(public name: string) { }

    abstract printCredits(): void;
}

class Bar extends Foo {
    ...
    printCredits(): void { // do stuff }
}
```

- Class Expressions 
```
class Foo { } // class statement 

let Musical = class extends Video {
    printCredits(): void { }
}

let mine = new Musical();

class Course extends class { title: string = '';  } { subject: string = ' ' }
```

# Modules 
- Relative
    - Use for your code
```
import { Laptop } from '/hardware'; 
import { Laptop } from './hardware'; 
import { Laptop } from '../hardware'; 
```

- Non relative 
    - Use for third party
```
import { Laptop } from 'hardware'; 
import * as $ from 'jquery'; 
```

- Resolution `tsc --moduleResolution Classic | Node`
- Classic 
    - Default for AMD, UMD, System, or ES2015 modules 
    - Less configurable
- Node
    - Defaults for CommonJS modules
    - Mirrors how node resolves modules (a little bit more complex)
    - Reads package.json - so more configurable

# Async
- Promises 
    - Native support in ES2015 (--target ES2015 or higher)
    - Api:  
        - then / catch
```
function doAsync(resolve, reject) {
    if(success) resolve(data);
    else reject(reason);
}

let p: Promise<string> = new Promise(doAsync);
let p: Promise<string> = new Promise((resolve, reject) => {
    if(success) resolve(data);
    else reject(reason);
});
```

# Generics
- Functions
```
function logAndReturn<T>(thing: T): T {
    console.log(thing);
    return thing;
}
```

- Interfaces
```
interface Inventory<T> {
    getNewest: () => T;
    addItem: (newItem: T) => void;
    getAllItems: () => Array<T>;
}
```

# Type Declaration Files 
- Good for putting javascript into Typescript
    - Declare types for all the stuff in the javascript library to match the use 
    - `.d.ts`
    - `@types/lodash`

# Decorators
- Typescript 5 is all new (vs older)
    - Support Javascript decorator proposal
    - Not compatible for earlier typescript 
    - Do not use `--experimentalDecorators` or `--emitDecoratorMetadata` flags
- Meta programming (functions that act on other code)
```
function logMethodInfo(origMethod: any, _context: ClassMethodDecoratorContext) {
    function replacementMethod(this: any, ...args: any[]) {

        console.log(`Decorated construct: ${_context.kind}`);
        console.log(`Decorated construct name: ${_context.name as string}`);

        const result = origMethod.call(this, ...args);
        return result;
    }
    return replacementMethod;
}

class Document {
    ...
    @logMethodInfo
    printItem(): void {
        console.log("Hello");
    }
}

let sportsDoc: Document = new Document('foo', 10, 'bar');
sportsDoc.printItem();

// Output
> Decorated construct: method 
> Decorated construct name: printItem 
> Hello
```







