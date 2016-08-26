package org.keplerproject.cgilua.servlet;

import java.io.File;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.keplerproject.luajava.LuaObject;
import org.keplerproject.luajava.LuaState;

/**
 * CGILuaServlet is a servlet that is responsible for launching cgilua
 * from a Java Web Server.
 * 
 * @author Thiago Ponte
 */
public class CGILuaServlet extends HttpServlet
{
  /** property names on config file */
  private static final String LOG_PARAM      = "log4j.config";
  private static final String LAUNCHER_PATH  = "cgilua.launcherpath";
  private static final String BASE_DIR       = "cgilua.basedir";
  private static final String POOL_SIZE      = "cgilua.poolsize";
  
  /** Logger object */
  private Logger              logger;
  /** CGI path on application */
  private String              cgiPath;

  /**
   * Init function. <br>
   * Responsible for creating the LuaState, loading the script and creating a
   * reference to the launch function.
   */
  public void init() throws ServletException
  {
    try
    {
      // starts log4j
      String file = getServletConfig().getInitParameter(LOG_PARAM);
      if (file != null)
        PropertyConfigurator.configure(file);
      else
        BasicConfigurator.configure();

      logger = Logger.getLogger("CGILua");

      // inits state pool
      file            = getServletConfig().getInitParameter(LAUNCHER_PATH);
      String baseDir  = getServletConfig().getInitParameter(BASE_DIR);
      String poolSize = getServletConfig().getInitParameter(POOL_SIZE);
      
      int maxStates;
      try
      {
        maxStates = Integer.parseInt(poolSize);
      }
      catch(NumberFormatException e)
      {
        maxStates = -1;
      }
      
      // log java.library.path
      String libraryPath = System.getProperty("java.library.path");
      logger.info("java.library.path=" + libraryPath);
      
      LuaStatePool.initPool(file, logger, baseDir, maxStates);
    }
    catch (Exception e)
    {
      throw new ServletException(e);
    }
  }

  public void destroy()
  {
    try
    {
      //pool.close();
      LuaStatePool.getPool().destroyPool();
    }
    catch (Exception ignore) {}
  }

  public void service(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException
  {
    launch(request, response);
  }

  /**
   * Method that calls the function launch from lua <br>
   * The launch function receives two parameters, the CGILuaRequest object and
   * the HttpResponse object.
   */
  private void launch(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException
  {
    CGILuaRequest cgiRequest = new CGILuaRequest(request, this);

    Object[] args = {cgiRequest, response};
    try
    {
      LuaState L = null;
      try
      {
        L = LuaStatePool.getPool().getState();
        LuaObject lLauncher = L.getLuaObject("launch");
        lLauncher.call(args);
      }
      finally
      {
        if (L != null)
          LuaStatePool.getPool().releaseState(L);
      }
    }
    catch (Exception e)
    {
      throw new ServletException(e);
    }
  }

  /**
   * Class that implements the request object from cgilua.
   *  
   * @author Thiago Ponte
   */
  protected class CGILuaRequest
  {
    private HttpServletRequest    request;
    private CGILuaServlet servlet;

    public CGILuaRequest(HttpServletRequest request,
        CGILuaServlet servlet)
    {
      this.request = request;
      this.servlet = servlet;
    }

    public byte[] getPostData(int n) throws IOException
    {
      byte[] bytes = new byte[n];

      request.getInputStream().read(bytes);

      return bytes;
    }

    public String serverVariable(String name)
    {
      String ret = null;

      if ("SERVER_NAME".equalsIgnoreCase(name))
        ret = request.getServerName();
      else if ("SERVER_PORT".equalsIgnoreCase(name))
        ret = String.valueOf(request.getServerPort());
      else if ("REQUEST_METHOD".equalsIgnoreCase(name))
        ret = request.getMethod();
      else if ("PATH_INFO".equalsIgnoreCase(name))
        ret = request.getPathInfo();
      else if ("PATH_TRANSLATED".equalsIgnoreCase(name))
      {
      	// TODO: read cgilua.scriptdir from web.xml and use if available
      	
      	// try to get path translated
      	// only works if there is a path info
      	// if servlet url-pattern is an "extension patter", like "*.lua", 
      	// path info is not available
      	ret = request.getPathTranslated();
      	if (ret != null) {
      		return ret;
      	}
      	
      	// get servlet context real path, servlet path, and concatenate
        String rootDir = servlet.getServletContext().getRealPath("/");
        if (rootDir != null) {
          	String servletPath = request.getServletPath();
          	ret = servletPath.replace('/', File.separatorChar);
          	if (File.separatorChar == ret.charAt(0)) {
          		ret = ret.substring(1);
          	}
          	ret = rootDir + ret;
          	return ret;
        }

        // cannot resolve, return null
        return ret;
      }
      else if ("SCRIPT_NAME".equalsIgnoreCase(name))
        ret = request.getServletPath();
      else if ("QUERY_STRING".equalsIgnoreCase(name))
        ret = request.getQueryString();
      else if ("REMOTE_HOST".equalsIgnoreCase(name))
        ret = request.getRemoteHost();
      else if ("REMOTE_ADDR".equalsIgnoreCase(name))
        ret = request.getRemoteAddr();
      else if ("REMOTE_HOST".equalsIgnoreCase(name))
        ret = request.getRemoteHost();
      else if ("AUTH_TYPE".equalsIgnoreCase(name))
        ret = request.getAuthType();
      else if ("REMOTE_USER".equalsIgnoreCase(name)
          || "REMOTE_IDENT".equalsIgnoreCase(name))
        ret = request.getRemoteUser();
      else if ("CONTENT_TYPE".equalsIgnoreCase(name))
        ret = request.getContentType();
      else if ("CONTENT_LENGTH".equalsIgnoreCase(name))
        ret = String.valueOf(request.getContentLength());
      else if ("HTTP_COOKIE".equalsIgnoreCase(name))
        ret = request.getHeader("Cookie");
      else if ("SERVER_SOFTWARE".equalsIgnoreCase(name))
        ret = "cgilua";
      else if ("GATEWAY_INTERFACE".equalsIgnoreCase(name))
        ret = "CGI/1.1";
      else if ("SERVER_PROTOCOL".equalsIgnoreCase(name))
        ret = "HTTP/1.1";

      if (ret == null)
        ret = "";

      return ret;
    }
  }
}
