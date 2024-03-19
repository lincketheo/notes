/**
 * A SubstringEntry is a substring of a string
 * and a boolean on whether it matches or doesn't match
 * ("match" usually means in regards to a search query)
 */
export interface SubstringEntry {
    text: string,
    matching: boolean,
}

/**
 * Computes a list of substring entries that match a given text
 * @param text The input text
 * @param search The search query
 * @returns If you appened v.text for each v in the ordered list, you'd get the original string
 */
export function splitSubstrings(text: string, search: string): SubstringEntry[] {
    return START(text, search)
}

/**
 * Modeled using Finite State Automata:
 *
 * States: START | MATCHING | NOTMATCHING | TERMINAL
 * Each transition alters the state of subLists
 */
function START(text: string, search: string): SubstringEntry[] {
    if (search.length === 0 || text.length === 0) {
        const subLists = [{
            text: text,
            matching: false,
        }] as SubstringEntry[]
        return TERMINAL(subLists)
    }
    if (search[0].toLowerCase() === text[0].toLowerCase()) {
        return MATCHING([], search, text, 1, 1, text[0])
    } else {
        return NOTMATCHING([], search, text, 1, text[0])
    }
}

function NOTMATCHING(subLists: SubstringEntry[], search: string, text: string, ti: number, subStr: string): SubstringEntry[] {
    if (ti === text.length) {
        subLists.push({
            text: subStr,
            matching: false,
        })
        return TERMINAL(subLists)
    }
    if (text[ti].toLowerCase() === search[0].toLowerCase()) {
        subLists.push({
            text: subStr,
            matching: false,
        })
        return MATCHING(subLists, search, text, 1, ti + 1, "" + text[ti])
    } else {
        return NOTMATCHING(subLists, search, text, ti + 1, subStr + text[ti])
    }
}

function MATCHING(subLists: SubstringEntry[], search: string, text: string, si: number, ti: number, subStr: string): SubstringEntry[] {
    if (si === search.length) {
        subLists.push({
            text: subStr,
            matching: true
        })
        if (ti === text.length) {
            return TERMINAL(subLists)
        }
        if (text[ti].toLowerCase() === search[0].toLowerCase()) {
            return MATCHING(subLists, search, text, 1, ti + 1, text[ti])
        } else {
            return NOTMATCHING(subLists, search, text, ti + 1, text[ti])
        }
    } else {
        if (ti === text.length) {
            subLists.push({
                text: subStr,
                matching: false
            })
            return TERMINAL(subLists)
        }
        if (text[ti].toLowerCase() !== search[si].toLowerCase()) {
            return NOTMATCHING(subLists, search, text, ti + 1, subStr + text[ti])
        } else {
            return MATCHING(subLists, search, text, si + 1, ti + 1, subStr + text[ti])
        }
    }
}

function TERMINAL(subLists: SubstringEntry[]) {
    return subLists
}
