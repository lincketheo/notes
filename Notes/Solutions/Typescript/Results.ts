export enum ResultType {
    Ok,
    Err
  }
  
  interface Ok<T> {
    type: ResultType.Ok,
    value: T
  }
  
  interface Err<E> {
    type: ResultType.Err,
    value: E
  }
  
  export type Result<T, E> = Ok<T> | Err<E>;
  
  export function Ok<T>(val: T): Ok<T> {
    return {value: val, type: ResultType.Ok} 
  }
  
  export function Err<E>(val: E): Err<E> {
    return {value: val, type: ResultType.Err} 
  }