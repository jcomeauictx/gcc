/* Copyright (C) 1998, 1999  Red Hat, Inc.

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */
 
package java.lang;
 
/**
 * @author Warren Levy <warrenl@cygnus.com>
 * @date September 8, 1998.  
 */
/* Written using online API docs for JDK 1.2 beta from http://www.javasoft.com.
 * Status:  Believed complete and correct.
 */
 
public interface Comparable
{
  public int compareTo(Object o) throws ClassCastException;
}
