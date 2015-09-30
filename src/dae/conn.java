/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dae;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

/**
 *
 * @author joebanc
 */
public final class conn {
    
    private Connection dbCon = null;
    private ResultSet rs = null;
    private Statement stmt = null;
       
    public conn(String sDb, String sUser, String sPwd, String sPort) {
        connect(sDb, sUser, sPwd, sPort);
    }
    public conn(String sSQL, String sDb, String sUser, String sPwd, String sPort) {
        connect(sDb, sUser, sPwd, sPort);
                
        try {
            //getting database connection to MySQL server
            stmt = dbCon.prepareStatement(sSQL);
            rs = stmt.executeQuery(sSQL);
            System.out.println("Query " +  sSQL + " executed.");
        } 
        catch (SQLException ex) {
            System.out.println(ex.getLocalizedMessage());
        }        
    }
   
    public void connect(String sDb, String sUser, String sPwd, String sPort) {
        String dbURL = String.format("jdbc:mysql://localhost:%s/%s", sPort, sDb);
        String username = sUser;
        String password = sPwd;
        
        //String socket = "/local/tmp/joebanc/mysql/thesock";//figure this out
        
        try {
            dbCon = DriverManager.getConnection(dbURL, username, password);
            //System.out.println("connection to " +  sDb + " executed.");
        } 
        catch (SQLException ex) {
            System.out.println(ex.getLocalizedMessage());
        }
    }
    /**
     *
     * @param sTable
     */
    public void openTable(String sTable) {
        try {
            if (dbCon==null) 
            {
                System.out.println("db not yet initialized.");
                return;
            }
            rs.close();
            //getting database connection to MySQL server
            String sSQL = "SELECT * FROM `" + sTable +" `;";
            stmt = dbCon.prepareStatement(sSQL);
            rs = stmt.executeQuery(sSQL);
        } 
        catch (SQLException ex) {
            System.out.println(ex.getLocalizedMessage());
        }
    }
    public int getCount() throws SQLException {
       
        if (rs==null || dbCon==null) 
        {
            System.out.println("db not yet initialized.");
            return -1;
        }
        
        int ct = 0;
        while (rs.next()) {
            ++ct;
            // Get data from the current row and use it
        }
        
        if (ct == 0) {
            System.out.println("No records found");
        }
        //System.out.println(count);
        return ct;
    }
    
    //static since no matter how many conn instances, there is only one connect method
    
    /**
     *
     * @return
     */
    public Connection GetCon() {
        return dbCon;
    }
    public ResultSet GetRs() {
        return rs;
    }
}
