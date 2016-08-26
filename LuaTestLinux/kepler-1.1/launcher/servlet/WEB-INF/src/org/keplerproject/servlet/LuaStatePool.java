package org.keplerproject.cgilua.servlet;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import org.apache.log4j.Logger;

import org.keplerproject.luajava.LuaException;
import org.keplerproject.luajava.LuaState;
import org.keplerproject.luajava.LuaStateFactory;

/**
 * @author Thiago Ponte
 */
public class LuaStatePool
{
  /** Default number of states that will be created */
  private static final int DEFAULT_MAX_STATES = 10;
  /** Singleton instance of the pool */
  private static LuaStatePool pool;
  /** Stores the states */
  private Set states;
  /** Object used to synchronize */
  private Object sync = new Object();
  
  /** private constructor */
  private LuaStatePool(String configFile, Logger logger, String rootDir, int maxStates) throws Exception
  {
    states = new HashSet();
    
    for (int i = 0; i < maxStates; i++)
    {
      LuaState L = LuaStateFactory.newLuaState();
      
      initState(L, configFile, logger, rootDir);
      states.add(L);
    }
  }
  
  /** Prepares the state */
  private void initState(LuaState L, String configFile, Logger logger, String rootDir)
  	throws Exception
  {
    L.openBasicLibraries();
    L.openDebug();
    L.openLoadLib();
    
    L.pushJavaObject(logger);
    L.setGlobal("logger");

    L.pushString(rootDir);
    L.setGlobal("cgilua_root");
    
    int err = 0;
    err = L.LloadFile(configFile);
    if (err == 0)
      err = L.pcall(0, 0, 0);
    
    if(err != 0)
    {
      String error = L.toString(-1);
      L.pop(1);
      throw new Exception(error);
    }
  }
  
  /** initializes pool */
  public static void initPool(String configFile, Logger logger, String rootDir, int maxStates)
  	throws Exception
  {
    if (pool == null)
    {
      if (maxStates <= 0)
        maxStates = DEFAULT_MAX_STATES;
      
      pool = new LuaStatePool(configFile, logger, rootDir, maxStates);
    }
  }
  
  /** to destroy the pool of states */
  public void destroyPool()
  {
    for (Iterator iter = states.iterator(); iter.hasNext();)
	  {
	    LuaState L = (LuaState) iter.next();
	    L.close();
	  }
    pool = null;
  }
  
  /** Get the single instance of the pool */
  public static LuaStatePool getPool()
  {
    return pool;
  }
  
  /** returns the state from the pool */
  public LuaState getState() throws LuaException
  {
    try
    {
      synchronized(sync)
      {
        while(states.isEmpty())
          sync.wait();
        
        LuaState L = (LuaState) states.toArray()[0];
        states.remove(L);
        
        return L;
      }
    }
    catch(Exception e)
    {
      throw new LuaException(e);
    }
  }
  
  /** release state */
  public void releaseState(LuaState L)
  {
    synchronized(sync)
    {
	    if (states.add(L))
	    {
	      sync.notify();
	    }
    }
  }
}