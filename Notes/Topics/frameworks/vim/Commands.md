# Practice:
H M L - top middle bottom of page
zz zb zt - center screen on where cursor is, move cursor to bottom / top (keeping line)

\} \{ - beginning / end of paragraph
\) \( - beginning / end of sentence

g - first line
G last line

ctrl b/f - move page up and down

\] + m or \[ + m - navigate functions

C-i: opposite of C-o

# Buffers
- :bnext 
- :bprevious 
- :buffer <buffer> 
- :bdelete <buffer>
- C-^ previous buffer
- :wqall (all buffers)

# Window
A viewport within a buffer
- :split <file>
- C-w h|j|k|l - move to window
- C-w H|J|K|L - move window
- C-w v - vertical split
- C-w s - horizontal split
- C-w c - close window 
- C-w o - current window the only one 

# Tab
- :tabnew <file>
- :tabclose
- :tabnext | `<number>gt` | `<number>gT`
- :tabprevious
- :tablast
- :tabfirst

# Sessions
- :mksession ~/<name>.vim
- :source ~/<name>.vim
