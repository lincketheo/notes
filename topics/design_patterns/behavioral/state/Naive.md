# Intent
State - internal state changes - changes class

# Naive

```
class Document 
    state: String 

    publish() {
        switch(state) {
            case "draft": {
                state = "moderation";
                break;
            }
            case "moderation": {
                if(user.role == "admin")
                    state = "published";
                break;
            }
            case "published": {
                break;
            }
        }
    }
```

# Solution

```
class Document {
    State state;
    render() {
        state.render();
    }
    publish() {
        state.publish();
    }
    changeState(state) {
        state = state;
    }
}

interface State {
    render();
    publish();
}

class Draft : State {
    Document document;
    render() {
        if(user.isAdmin or user.isAuthor) {
            // show the doc
        } else {
            // error
        }
    }
    publish() {
        if(user.isAdmin) {
            document.changeState(new Published(document));
        } else {
            document.changeState(new Moderation(document));
        }
    }
}
```
