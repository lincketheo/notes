# Dependency Injection

## Without Dependency Injection
```python
class Car:
    def __init__(self):
        self.engine = Engine()

    def start(self):
        return self.engine.start()
```

## Constructor Injection
```python
class Car:
    def __init__(self, engine):
        self.engine = engine

    def start(self):
        return self.engine.start()
```

## Setter Injection
```python
class Car:
    def __init__(self):
        self.engine = None

    def set_engine(self, engine):
        self.engine = engine
```

## Method parameter Injection
```python
class Car:
    def start(self, engine):
        return engine.start()
```