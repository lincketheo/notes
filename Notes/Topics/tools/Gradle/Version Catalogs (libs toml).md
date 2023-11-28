
https://docs.gradle.org/current/userguide/platforms.html

## Version Catalog
List of dependencies that a client can pick from

Example:
```kotlin
dependencies {
	implementation(libs.groovy.core)
}
```
`libs`: _catalog_
`groovy`: dependency

### Declaring
`settings.gradle.kts`
```kotlin
dependencyResolutionManagement {
    versionCatalogs {
        create("libs") {
            library("groovy-core", "org.codehaus.groovy:groovy:3.0.5")
            library("groovy-json", "org.codehaus.groovy:groovy-json:3.0.5")
            library("groovy-nio", "org.codehaus.groovy:groovy-nio:3.0.5")
            library("commons-lang3", "org.apache.commons", "commons-lang3").version {
                strictly("[3.8, 4.0[")
                prefer("3.9")
            }
        }
    }
}
```

The following aliases
`guava`, `groovy-core`, `groovy-xml`, `groovy-json`, `androidx.awesome.lib`

Generates type safe accessors:
- `libs.guava`
- `libs.groovy.core`
- `libs.groovy.xml`
- `libs.groovy.json`
- `libs.androidx.awesome.lib`

### Same Version Numbers
```kotlin
dependencyResolutionManagement {
    versionCatalogs {
        create("libs") {
            version("groovy", "3.0.5")
            version("checkstyle", "8.37")
            library("groovy-core", "org.codehaus.groovy", "groovy").versionRef("groovy")
            library("groovy-json", "org.codehaus.groovy", "groovy-json").versionRef("groovy")
            library("groovy-nio", "org.codehaus.groovy", "groovy-nio").versionRef("groovy")
            library("commons-lang3", "org.apache.commons", "commons-lang3").version {
                strictly("[3.8, 4.0[")
                prefer("3.9")
            }
        }
    }
}
```

**Usage:**
```kotlin
dependencies {
    implementation(libs.groovy.core)
    implementation(libs.groovy.json)
    implementation(libs.groovy.nio)
}
```


### Dependency Bundles
```kotlin
dependencyResolutionManagement {
    versionCatalogs {
        create("libs") {
            version("groovy", "3.0.5")
            version("checkstyle", "8.37")
            library("groovy-core", "org.codehaus.groovy", "groovy").versionRef("groovy")
            library("groovy-json", "org.codehaus.groovy", "groovy-json").versionRef("groovy")
            library("groovy-nio", "org.codehaus.groovy", "groovy-nio").versionRef("groovy")
            library("commons-lang3", "org.apache.commons", "commons-lang3").version {
                strictly("[3.8, 4.0[")
                prefer("3.9")
            }
            bundle("groovy", listOf("groovy-core", "groovy-json", "groovy-nio"))
        }
    }
}
```

**Usage:**
```kotlin
dependencies {
    implementation(libs.bundles.groovy)
}
```

### Plugins
```kotlin
dependencyResolutionManagement {
    versionCatalogs {
        create("libs") {
            plugin("versions", "com.github.ben-manes.versions").version("0.45.0")
        }
    }
}
```

**Usage:**
```kotlin
plugins {
    `java-library`
    checkstyle
    alias(libs.plugins.versions)
}
```

## libs.versions.toml
Gradle automatically picks up `libs.versions.toml` inside the `gradle` directory
**Base Example:**
```toml
[versions]
groovy = "3.0.5"
checkstyle = "8.37"

[libraries]
groovy-core = { module = "org.codehaus.groovy:groovy", version.ref = "groovy" }
groovy-json = { module = "org.codehaus.groovy:groovy-json", version.ref = "groovy" }
groovy-nio = { module = "org.codehaus.groovy:groovy-nio", version.ref = "groovy" }
commons-lang3 = { group = "org.apache.commons", name = "commons-lang3", version = { strictly = "[3.8, 4.0[", prefer="3.9" } }
bundles]
groovy = ["groovy-core", "groovy-json", "groovy-nio"]

[plugins]
versions = { id = "com.github.ben-manes.versions", version = "0.45.0" }
```

