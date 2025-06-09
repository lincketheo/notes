# Folding
`zo`   - Open a fold  
`zc`   - Close a fold  
`zO`   - Open all nested folds under cursor  
`zC`   - Close all nested folds under cursor  
`za`   - Toggle fold open/closed  
`zA`   - Toggle recursively (open/close all nested folds)

`zn`   - Disable folding (shows all lines, folds are still defined)
`zN`   - Re-enable folding

`zR`   - Open all folds in buffer  
`zM`   - Close all folds in buffer

`zd`   - Delete fold under cursor  
`zE`   - Delete all folds in buffer

`[z`   - Jump to start of current/open fold  
`]z`   - Jump to end of current/open fold  

`:set foldlevel=0`    - Close all folds (folds at level 0 shown)  
`:set foldlevel=99`    - Open all folds (or use `zR`)

`zf{motion}`           - Create a fold using motion (e.g., `zf5j`)  
`zf` in Visual Mode    - Create a fold over selected lines  
`:foldopen [N]`        - Open fold(s)  
`:foldclose [N]`       - Close fold(s)  
`:foldtoggle [N]`      - Toggle fold(s)  

# Sessions 
`:mksession <PATH>`
`:source <PATH>`
`nvim -S <PATH>`
