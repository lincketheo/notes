## Example:
```
[Desktop Entry]
Type=Application
Name=Obsidian
Exec=/home/theo/Development/builds/obsidian/Obsidian-1.7.7.AppImage
Icon=/home/theo/Development/builds/obsidian/obsidian.png
Comment=Obsidian App 
Categories=Utility;
Terminal=false
X-Desktop-File-Install-Version=0.26
```

```
$ desktop-file-validate Obsidian.desktop
$ desktop-file-install --dir=$HOME/.local/share/applications Obsidian.desktop
$ update-desktop-database ~/.local/share/applications
$ sudo update-desktop-database /usr/share/applications
```