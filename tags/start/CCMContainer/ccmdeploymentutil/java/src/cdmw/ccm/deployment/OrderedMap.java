/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


package cdmw.ccm.deployment;


/**
 * This implementation of the <tt>java.util.Map</tt> interface provides all 
 * of the optional map operations, and permits <tt>null</tt> values and the 
 * <tt>null</tt> key. 
 * This class guarantees that the map keep the order in wich the entries are
 * added. If an already mapped entry is replaced, this entry is moved at
 * the end of the map.
 *
 * <b>Note that this implementation is not synchronized.</b> If multiple
 * threads access this map concurrently, and at least one of the threads
 * modifies the map structurally, it <i>must</i> be synchronized externally.
 * (A structural modification is any operation that adds or deletes one or
 * more mappings; merely changing the value associated with a key that an
 * instance already contains is not a structural modification.)  This is
 * typically accomplished by synchronizing on some object that naturally
 * encapsulates the map.  If no such object exists, the map should be
 * "wrapped" using the <tt>Collections.synchronizedMap</tt> method.  This is
 * best done at creation time, to prevent accidental unsynchronized access to
 * the map: <pre> Map m = Collections.synchronizedMap(new HashMap(...));
 * </pre><p/>
 *
 * The iterators returned by all of this class's "collection view methods" are
 * <i>fail-fast</i>: if the map is structurally modified at any time after the
 * iterator is created, in any way except through the iterator's own
 * <tt>remove</tt> or <tt>add</tt> methods, the iterator will throw a
 * <tt>ConcurrentModificationException</tt>.  Thus, in the face of concurrent
 * modification, the iterator fails quickly and cleanly, rather than risking
 * arbitrary, non-deterministic behavior at an undetermined time in the
 * future.
 *
 * @see java.util.Map
 */
 
public class OrderedMap 
    extends java.util.AbstractMap 
    implements java.util.Map, Cloneable, java.io.Serializable
{
    
    private java.util.List keys;
    private java.util.List values;
    
    private transient java.util.Set entrySet;
    
    /**
     * Constructs a new, empty map.
     */    
    public OrderedMap() {
        keys = new java.util.ArrayList();
        values = new java.util.ArrayList();
        entrySet = null;
    }
    
    /**
     * Constructs a new map with the same mappings as the given map, ordered into
     * the same order than the t.entrySet() result.
     *
     * @param t the map whose mappings are to be placed in this map.
     */    
     public OrderedMap(java.util.Map t) {
        this();
        putAll(t);
    }
        
    
    // Query Operations


    /**
     * Returns <tt>true</tt> if this map contains a mapping for the specified
     * key.
     *
     * @param key key whose presence in this map is to be tested.
     * @return <tt>true</tt> if this map contains a mapping for the specified
     * key.
     */
    public boolean containsKey(Object key) {
        return keys.contains(key);
    }
    

    /**
     * Returns <tt>true</tt> if this map maps one or more keys to the
     * specified value.  More formally, returns <tt>true</tt> if and only if
     * this map contains at least one mapping to a value <tt>v</tt> such that
     * <tt>(value==null ? v==null : value.equals(v))</tt>.  This operation
     * will probably require time linear in the map size for most
     * implementations of the <tt>Map</tt> interface.
     *
     * @param value value whose presence in this map is to be tested.
     * @return <tt>true</tt> if this map maps one or more keys to the
     *         specified value.
     */
    public boolean containsValue(Object value) {
        return values.contains(value);
    }
    

    /**
     * Returns the value to which this map maps the specified key.  Returns
     * <tt>null</tt> if the map contains no mapping for this key.  A return
     * value of <tt>null</tt> does not <i>necessarily</i> indicate that the
     * map contains no mapping for the key; it's also possible that the map
     * explicitly maps the key to <tt>null</tt>.  The <tt>containsKey</tt>
     * operation may be used to distinguish these two cases.
     *
     * @param key key whose associated value is to be returned.
     * @return the value to which this map maps the specified key, or
     *	       <tt>null</tt> if the map contains no mapping for this key.
     * 
     * @see #containsKey(Object)
     */
    public Object get(Object key) {
        int i = keys.indexOf(key);
        
        if (i == -1) {
            return null;
        } else {
            return values.get(i);
        }
    }
    

    // Modification Operations

    /**
     * Associates the specified value with the specified key in this map
     * (optional operation).  If the map previously contained a mapping for
     * this key, the old value is replaced.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     * @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the map previously associated <tt>null</tt>
     *	       with the specified key, if the implementation supports
     *	       <tt>null</tt> values.
     */
    public Object put(Object key, Object value) {
        // remove old mapping (if exists) before inserting the new one
        // at the end of the map
        Object oldValue = remove(key);

        keys.add(key);
        values.add(value);
        return oldValue;
    }
    

    /**
     * Removes the mapping for this key from this map if present (optional
     * operation).
     *
     * @param key key whose mapping is to be removed from the map.
     * @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the map previously associated <tt>null</tt>
     *	       with the specified key, if the implementation supports
     *	       <tt>null</tt> values.
     */
    public Object remove(Object key) {
        Object oldValue = null;
        int i = keys.indexOf(key);

        if (i != -1) {
            keys.remove(i);
            oldValue = values.remove(i);
        }

        return oldValue;
    }



    // Views

    /**
     * Returns a collection view of the values contained in this map.  The
     * collection is backed by the map, so changes to the map are reflected in
     * the collection, and vice-versa.  (If the map is modified while an
     * iteration over the collection is in progress, the results of the
     * iteration are undefined.)  The collection supports element removal,
     * which removes the corresponding entry from the map, via the
     * <tt>Iterator.remove</tt>, <tt>Collection.remove</tt>,
     * <tt>removeAll</tt>, <tt>retainAll</tt> and <tt>clear</tt> operations.
     * It does not support the <tt>add</tt> or <tt>addAll</tt> operations.<p>
     *
     * @return a collection view of the values contained in this map.
     */
    public java.util.Collection values() {
        return values;
    }
    
    /**
     * Returns a set view of the mappings contained in this map.  Each element
     * in the returned set is a <tt>Map.Entry</tt>.  The set is backed by the
     * map, so changes to the map are reflected in the set, and vice-versa.
     * If the map is modified while an iteration over the set is in progress,
     * the results of the iteration are undefined.  The set supports element
     * removal, which removes the corresponding mapping from the map, via the
     * <tt>Iterator.remove</tt>, <tt>Set.remove</tt>, <tt>removeAll</tt>,
     * <tt>retainAll</tt> and <tt>clear</tt> operations.  It does not support
     * the <tt>add</tt> or <tt>addAll</tt> operations.
     *
     * @return a set view of the mappings contained in this map.
     */
    public java.util.Set entrySet() {
    
        if (entrySet == null) {

            entrySet = new java.util.AbstractSet() {
                
                public java.util.Iterator iterator() {
                    return new java.util.Iterator() {
                        private java.util.Iterator keyIt = keys.iterator();
                        private java.util.Iterator valIt = values.iterator();
                        
                        public boolean hasNext() {
                            return keyIt.hasNext();
                        }
                        
                        public Object next() {
                            Object key = keyIt.next();
                            Object val = valIt.next();
                            return new Entry(key, val);
                        }
                        
                        public void remove() {
                            keyIt.remove();
                            valIt.remove();
                        }
                    }; // end Iterator definition
                } // end iterator()

                public int size() {
                    return keys.size();
                }
                
                public boolean contains(Object o) {
                    if (!(o instanceof java.util.Map.Entry))
                        return false;
                        
                    java.util.Map.Entry entry = (java.util.Map.Entry) o;
                    if (! keys.contains(entry.getKey()))
                        return false;
                        
                    Object value = OrderedMap.this.get(entry.getKey());
                    return (value==null ? entry.getValue()==null 
                                        : value.equals(entry.getValue()));
                }
                
                public boolean remove(Object o) {
                    if (!contains(o))
                        return false;
                        
                    java.util.Map.Entry entry = (java.util.Map.Entry) o;
                    OrderedMap.this.remove(entry.getKey());
                    return true;
                }
                
                public void clear() {
                    keys.clear();
                    values.clear();
                }

            }; // end of AbstractSet definition
        } // end of if (entrySet == null)

        return entrySet;
    }
    


    
    /**
     * HashMap collision list entry.
     */
    private static class Entry implements java.util.Map.Entry {
	    Object key;
    	Object value;
    
    	Entry(Object key, Object value) {
    	    this.key = key;
    	    this.value = value;
    	}
    
    	protected Object clone() {
    	    return new Entry(key, value);
    	}
    
    	// Map.Entry Ops 
    
    	public Object getKey() {
    	    return key;
    	}
    
    	public Object getValue() {
    	    return value;
    	}
    
    	public Object setValue(Object value) {
    	    Object oldValue = this.value;
    	    this.value = value;
    	    return oldValue;
    	}
    
    	public boolean equals(Object o) {
    	    if (!(o instanceof java.util.Map.Entry))
    		return false;
    	    java.util.Map.Entry e = (java.util.Map.Entry)o;
    
    	    return (key==null ? e.getKey()==null : key.equals(e.getKey())) &&
    	           (value==null ? e.getValue()==null : value.equals(e.getValue()));
    	}
    
    	public int hashCode() {
    	    return (key==null ? 0 : key.hashCode()) ^ 
    	           (value==null ? 0 : value.hashCode());
    	}
    
    	public String toString() {
    	    return key+"="+value;
    	}
    }
    
    
    // additional operations
    
    /**
     * Returns a ListIterator of the values in the map (in same order 
     * the entries have been added).
     * 
     * @return a ListIterator of the values in the map
     */
    public java.util.ListIterator valuesListIterator() {
        return values.listIterator();
    }
    
    /**
     * Returns a ListIterator of the values in the map (in same order 
     * the entries have been added), starting at the specified position in this list
     * 
     * @param index index of first element to be returned from the list iterator 
     *        (by a call to the next method). 
     *
     * @return a ListIterator of the values in the 
     */
    public java.util.ListIterator valuesListIterator(int index) {
        return values.listIterator(index);
    }
    
    
}


