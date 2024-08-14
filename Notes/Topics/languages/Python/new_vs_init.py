class Person:
    def __new__(cls, name, age):
        print("Creating a new Person object")
        instance = super().__new__(cls)
        return instance

    def __init__(self, name, age):
        print("Initializing the Person object")
        self.name = name
        self.age = age

person1 = Person("John Doe", 30)
person2 = Person("John Doe", 30)
